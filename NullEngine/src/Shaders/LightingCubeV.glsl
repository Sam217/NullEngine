#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;

layout (std140, binding = 0) uniform matrixVP {
    mat4 view;
    mat4 projection;
};

uniform vec3 lightPos;

out VS_OUT {
    vec3 Normal;
    vec3 FragPos;
    vec3 LightPos;
    vec2 TexCoords;
} vs_out;


void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.LightPos = lightPos;
    //LightPos = vec3(view * model * vec4(lightPos, 1.0));
    vs_out.TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    //gl_Position = projection * view * model * vec4(FragPos, 1.0);
} 