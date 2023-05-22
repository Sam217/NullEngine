#version 330 core
#define NR_POINT_LIGHTS 4

out vec4 FragColor;

in VS_OUT {
    vec3 Normal;
    vec3 FragPos;
    vec3 LightPos;
    vec2 TexCoords;
} ps_in;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3  position;
    vec3  direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emissive;
    float shininess;
};

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

uniform Material material;
uniform vec3 viewPos;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(ps_in.Normal);
    vec3 viewDir = normalize(viewPos - ps_in.FragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, ps_in.FragPos, viewDir);

    // for the one lonely lightCube :_)
    result += CalcPointLight(pointLight, norm, ps_in.FragPos, viewDir);

    //vec3 emissive = (1 - diff) * floor(vec3(1.0f) - texture(material.specular, TexCoords).rgb) * texture(material.emissive, TexCoords).rgb;
    //vec3 emissive = vec3(0);
    result += CalcSpotLight(spotLight, norm, ps_in.FragPos, viewDir);

    // resulting lighting
    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, ps_in.TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, ps_in.TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, ps_in.TexCoords));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // calculate diffuse lighting
    vec3 lightDir = normalize(light.position - fragPos);

    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, ps_in.TexCoords)));

    // specular lighting
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (spec * texture(material.specular, ps_in.TexCoords)).rgb * light.specular;

    return (diffuse + specular) ;//* attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // calculate spotLight influence
    vec3 spLightDir = normalize(light.position - fragPos);
    float theta = dot(spLightDir, normalize(-light.direction));

    vec3 spotLightRes = vec3(0.0);
    if(theta > light.outerCutOff)
    {
        // do lighting calculations
        float dist = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
        //vec3 lightDir = normalize(-light.direction);
        float diff = max(dot(normal, spLightDir), 0.0);
        vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, ps_in.TexCoords)));

        // specular lighting
        vec3 reflectDir = reflect(normalize(light.direction), normal);

        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = (spec * texture(material.specular, ps_in.TexCoords)).rgb * light.specular;

        float epsilon = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

        spotLightRes = (diffuse + specular) * attenuation * intensity;
        return spotLightRes;
    }

    return spotLightRes;
}