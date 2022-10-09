#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;
uniform vec3 lightPos; 
out vec3 Normal;
out vec3 FragPos;
out vec3 LightPos;

out float diffAmount;
out float specularAmount;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(view * model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(view * model))) * aNormal;
    LightPos = lightPos;
    //LightPos = vec3(view * model * vec4(lightPos, 1.0));

    // calculate diffuse lighting
    vec3 norm = normalize(Normal);
    //vec3 lightDir = normalize(FragPos - lightPos);
    vec3 lightDir = normalize(LightPos - FragPos);

    // calculate Diffuse amount
    diffAmount = max(dot(norm, lightDir), 0.0);

    // specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 

    specularAmount = pow(max(dot(viewDir, reflectDir), 0.0), 256);
} 