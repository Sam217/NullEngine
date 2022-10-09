#version 330 core
in float diffAmount;
in float specularAmount;

in vec3 Normal; 
in vec3 FragPos; 
in vec3 LightPos;
out vec4 FragColor;
  
uniform float specularIntensity;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
    // calculate diffuse lighting
    vec3 diffuse = diffAmount * lightColor;

    // calculate ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // specular lighting
    float specularStrength = 0.5 + specularIntensity;
    vec3 specular = specularStrength * specularAmount * lightColor; 

    // resulting lighting
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
} 