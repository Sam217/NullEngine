#version 430 core
out vec4 FragColor;

in VS_OUT {
    vec3 Normal;
    vec3 FragPos;
    vec3 LightPos;
    vec2 TexCoords;
} ps_in;

uniform vec3 lightColor;

void main()
{
    FragColor = vec4(lightColor, 1.0); // set all 4 vector values to 1.0
}