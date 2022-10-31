#version 330 core
in vec3 Normal; 
in vec3 FragPos; 
in vec3 LightPos;
out vec4 FragColor;

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light; 

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emissive;
    float shininess;
};

in vec2 TexCoords;
  
uniform Material material;
  
uniform vec3 viewPos;

void main()
{
    // calculate ambient lighting
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    // calculate diffuse lighting
    vec3 norm = normalize(Normal);
    //vec3 lightDir = normalize(FragPos - lightPos);
    vec3 lightDir = normalize(LightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));

    // specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (spec * texture(material.specular, TexCoords)).rgb * light.specular; 

    vec3 emissive = (1 - diff) * floor(vec3(1.0f) - texture(material.specular, TexCoords).rgb) * texture(material.emissive, TexCoords).rgb;

    // resulting lighting
    vec3 result = (ambient + diffuse + specular + emissive);
    FragColor = vec4(result, 1.0);
} 