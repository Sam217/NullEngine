#version 330 core
in vec3 Normal; 
in vec3 FragPos; 
in vec3 LightPos;
out vec4 FragColor;

struct Light {
    //vec3 position;
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


struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emissive;
    float shininess;
};

in vec2 TexCoords;
  
uniform Light light; 
uniform PointLight pointLight; 
uniform Material material;
uniform vec3 viewPos;

void main()
{
    // calculate ambient lighting
    vec3 ambient = pointLight.ambient * vec3(texture(material.diffuse, TexCoords));

    // calculate diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(pointLight.position - FragPos);

    float dist = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * dist + pointLight.quadratic * (dist * dist)); 
    //vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = pointLight.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));

    // specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (spec * texture(material.specular, TexCoords)).rgb * pointLight.specular; 

    vec3 emissive = (1 - diff) * floor(vec3(1.0f) - texture(material.specular, TexCoords).rgb) * texture(material.emissive, TexCoords).rgb;
    //vec3 emissive = vec3(0);

    // resulting lighting
    vec3 result = (ambient + diffuse + specular) * attenuation;// + emissive;
    FragColor = vec4(result, 1.0);
} 