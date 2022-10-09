#version 330 core
in vec3 Normal; 
in vec3 FragPos; 
in vec3 LightPos;
out vec4 FragColor;
  
uniform float specularIntensity;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main()
{
    // calculate diffuse lighting
    vec3 norm = normalize(Normal);
    //vec3 lightDir = normalize(FragPos - lightPos);
    vec3 lightDir = normalize(LightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // calculate ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // specular lighting
    float specularStrength = 0.5 + specularIntensity;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
    vec3 specular = specularStrength * spec * lightColor; 

    // resulting lighting
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
} 