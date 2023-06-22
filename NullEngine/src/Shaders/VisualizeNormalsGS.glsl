#version 430 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

layout (std140, binding = 0) uniform matrixVP {
    mat4 view;
    mat4 projection;
};

in VS_OUT {
    vec3 Normal;
    vec3 FragPos;
    vec3 LightPos;
    vec2 TexCoords;
} gs_in[];

out GS_OUT {
    vec3 Normal;
    vec3 FragPos;
    vec3 LightPos;
    vec2 TexCoords;
} gs_out;

const float MAGNITUDE = 0.4;
  
void GenerateLine(int index)
{
    gl_Position = projection * gl_in[index].gl_Position;

    gs_out.Normal = gs_in[index].Normal;
    gs_out.FragPos = gs_in[index].FragPos;
    gs_out.LightPos = gs_in[index].LightPos;
    gs_out.TexCoords = gs_in[index].TexCoords;

    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position + 
                                vec4(gs_in[index].Normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
} 