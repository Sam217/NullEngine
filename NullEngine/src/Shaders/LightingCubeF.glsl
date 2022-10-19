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
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;
  
uniform float specularIntensity;
uniform vec3 viewPos;

void main()
{
    // calculate ambient lighting
    vec3 ambient = light.ambient * material.ambient;

    // calculate diffuse lighting
    vec3 norm = normalize(Normal);
    //vec3 lightDir = normalize(FragPos - lightPos);
    vec3 lightDir = normalize(LightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularIntensity * (spec * material.specular) * light.specular; 

    // resulting lighting
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
} 