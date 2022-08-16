#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
//layout (location = 1) in vec3 aColor;
layout (location = 1) in vec3 aColor;
//layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

//out vec4 vertexColor;
//out vec4 vertexColor2;

uniform mat4 transform;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float rotation;

uniform float xoffset;

void main()
{
	float cos60 = 1.0/2.0;
	float sin60 = 1.7320508076/2.0;
	float pi = 3.141592653589793238;
   
	//gl_Position = vec4(aPos.x*cos(rotation) + aPos.y*sin(rotation) + xoffset, aPos.y*cos(rotation) - aPos.x*sin(rotation), aPos.z, 1.0);
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	ourColor = aColor;
	TexCoord = aTexCoord;
	//float rot = 65 * pi / 180.0;
   //gl_Position = vec4(aPos.x*cos(rot) + aPos.y*sin(rot), aPos.y*cos(rot) - aPos.x*sin(rot), aPos.z, 1.0);
   //gl_Position = vec4(aPos.x + xoffset, -aPos.y, aPos.z, 1.0);
//   vertexColor = vec4(aColor, 1.0);
//
//   vertexColor2 = vec4(aPos, 1.0);
}