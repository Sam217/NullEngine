#version 430 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 Normal;
    vec3 FragPos;
    vec3 LightPos;
    vec2 TexCoords;
} gs_in[];

//out GS_OUT {
//    vec3 Normal;
//    vec3 FragPos;
//    vec3 LightPos;
//    vec2 TexCoords;
//} gs_out;

out vec3 Normal;
out vec3 FragPos;
out vec3 LightPos;
out vec2 TexCoords;

uniform float time;

vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude; 
    return position + vec4(direction, 0.0);
}

vec3 GetNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
} 

void main()
{    

    //vec3 normal = gs_in[0].Normal; //GetNormal();
    vec3 normal = GetNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);

//    gs_out.Normal = gs_in[0].Normal;
//    gs_out.FragPos = gs_in[0].FragPos;
//    gs_out.LightPos = gs_in[0].LightPos;
//
//    gs_out.TexCoords = gs_in[0].TexCoords;

    Normal = gs_in[0].Normal;
    FragPos = gs_in[0].FragPos;
    LightPos = gs_in[0].LightPos;

    TexCoords = gs_in[0].TexCoords;

    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, normal);
    
//    gs_out.Normal = gs_in[1].Normal;
//    gs_out.FragPos = gs_in[1].FragPos;
//    gs_out.LightPos = gs_in[1].LightPos;
//
//    gs_out.TexCoords = gs_in[1].TexCoords;

    Normal = gs_in[1].Normal;
    FragPos = gs_in[1].FragPos;
    LightPos = gs_in[1].LightPos;

    TexCoords = gs_in[1].TexCoords;

    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, normal);
    
//    gs_out.Normal = gs_in[2].Normal;
//    gs_out.FragPos = gs_in[2].FragPos;
//    gs_out.LightPos = gs_in[2].LightPos;
//
//    gs_out.TexCoords = gs_in[2].TexCoords;

    Normal = gs_in[2].Normal;
    FragPos = gs_in[2].FragPos;
    LightPos = gs_in[2].LightPos;

    TexCoords = gs_in[2].TexCoords;

    EmitVertex();
    EndPrimitive();
}