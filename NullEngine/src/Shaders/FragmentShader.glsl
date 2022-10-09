#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float ratio;

//in vec4 vertexColor;
//in vec4 vertexColor2;

void main()
{
	//vec2 faceCoord = vec2(1.0-TexCoord.x, TexCoord.y);
	vec2 faceCoord = vec2(TexCoord.x, TexCoord.y);
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, faceCoord), ratio);
}