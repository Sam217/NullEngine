#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

//in vec4 vertexColor;
//in vec4 vertexColor2;

void main()
{
	FragColor = texture(ourTexture, TexCoord);
}