#include <string>

std::string simpleVScode = R"(
    #version 430 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoords;

    out vec2 TexCoords;
    uniform mat4 transform;

    void main()
    {
      gl_Position = transform * vec4(aPos.x, aPos.y, 0.0, 1.0);
      TexCoords = aTexCoords;
    }
  )";

std::string simpleFScode = R"(
    #version 430 core
    out vec4 FragColor;
    in vec2 TexCoords;
    uniform sampler2D screenTexture;

    void main()
    {
      FragColor = texture(screenTexture, TexCoords);
    }
  )";

std::string simpleFSnegative = R"(
    #version 430 core
    out vec4 FragColor;
    in vec2 TexCoords;
    uniform sampler2D screenTexture;

    void main()
    {
      FragColor = vec4(vec3(1.0f - texture(screenTexture, TexCoords)), 1.0);
    }
  )";

std::string simpleFSgscale = R"(
    #version 430 core
    out vec4 FragColor;
    in vec2 TexCoords;
    uniform sampler2D screenTexture;

    void main()
    {
      vec3 texColor = texture(screenTexture, TexCoords).rgb;
      float avg = (texColor.r + texColor.g + texColor.b) / 3.0f;
      FragColor = vec4(avg, avg, avg, 1.0);
    })";

std::string simpleFSgscaleW = R"(
    #version 430 core
    out vec4 FragColor;
    in vec2 TexCoords;
    uniform sampler2D screenTexture;

    void main()
    {
      FragColor = texture(screenTexture, TexCoords);
      float avg = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
      FragColor = vec4(avg, avg, avg, 1.0);
    }
  )";

std::string fsSharpen = R"(
    #version 430 core
    out vec4 FragColor;
    in vec2 TexCoords;
    uniform sampler2D screenTexture;

    const float offset = 1.0 / 300.0;

    void main()
    {
      vec2 offsets[9] = vec2[](
          vec2(-offset,  offset), // top-left
          vec2( 0.0f,    offset), // top-center
          vec2( offset,  offset), // top-right
          vec2(-offset,  0.0f),   // center-left
          vec2( 0.0f,    0.0f),   // center-center
          vec2( offset,  0.0f),   // center-right
          vec2(-offset, -offset), // bottom-left
          vec2( 0.0f,   -offset), // bottom-center
          vec2( offset, -offset)  // bottom-right
      );

      float kernel[9] = float[](
          -1, -1, -1,
          -1,  9, -1,
          -1, -1, -1
      );

      vec3 sampleTex[9];
      for(int i = 0; i < 9; i++)
      {
          sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
      }
      vec3 col = vec3(0.0);
      for(int i = 0; i < 9; i++)
          col += sampleTex[i] * kernel[i];

      FragColor = vec4(col, 1.0);
      }
  )";

std::string fsBlur = R"(
    #version 430 core
    out vec4 FragColor;
    in vec2 TexCoords;
    uniform sampler2D screenTexture;

    const float offset = 1.0 / 300.0;

    void main()
    {
      vec2 offsets[9] = vec2[](
          vec2(-offset,  offset), // top-left
          vec2( 0.0f,    offset), // top-center
          vec2( offset,  offset), // top-right
          vec2(-offset,  0.0f),   // center-left
          vec2( 0.0f,    0.0f),   // center-center
          vec2( offset,  0.0f),   // center-right
          vec2(-offset, -offset), // bottom-left
          vec2( 0.0f,   -offset), // bottom-center
          vec2( offset, -offset)  // bottom-right
      );

      float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
      );

      vec3 sampleTex[9];
      for(int i = 0; i < 9; i++)
      {
          sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
      }
      vec3 col = vec3(0.0);
      for(int i = 0; i < 9; i++)
          col += sampleTex[i] * kernel[i];

      FragColor = vec4(col, 1.0);
      }
  )";

std::string fsEdge = R"(
    #version 430 core
    out vec4 FragColor;
    in vec2 TexCoords;
    uniform sampler2D screenTexture;

    const float offset = 1.0 / 300.0;

    void main()
    {
      vec2 offsets[9] = vec2[](
          vec2(-offset,  offset), // top-left
          vec2( 0.0f,    offset), // top-center
          vec2( offset,  offset), // top-right
          vec2(-offset,  0.0f),   // center-left
          vec2( 0.0f,    0.0f),   // center-center
          vec2( offset,  0.0f),   // center-right
          vec2(-offset, -offset), // bottom-left
          vec2( 0.0f,   -offset), // bottom-center
          vec2( offset, -offset)  // bottom-right
      );

      float kernel[9] = float[](
           1,  1,  1,
           1, -8,  1,
           1,  1,  1
      );

      vec3 sampleTex[9];
      for(int i = 0; i < 9; i++)
      {
          sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
      }
      vec3 col = vec3(0.0);
      for(int i = 0; i < 9; i++)
          col += sampleTex[i] * kernel[i];

      FragColor = vec4(col, 1.0);
      }
  )";

std::string skyBoxVSsrc = R"(
      #version 430 core
      layout (location = 0) in vec3 aPos;

      out vec3 TexCoords;

      layout (std140, binding = 0) uniform matrixVP {
        mat4 view;
        mat4 projection;
      };

      uniform mat4 skyBoxView;

      void main()
      {
          TexCoords = aPos;
          gl_Position = projection * skyBoxView * vec4(aPos, 1.0);
      }
  )";

std::string skyBoxFSsrc = R"(
    #version 430 core
    out vec4 FragColor;

    in vec3 TexCoords;

    uniform samplerCube skybox;

    void main()
    {
        FragColor = texture(skybox, TexCoords);
    }
  )";

std::string cmReflectVs = R"(
    #version 430 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    layout (location = 2) in vec2 aTexCoord;

    out vec3 Normal;
    out vec3 Position;

    uniform mat4 model;
    layout (std140, binding = 0) uniform matrixVP {
        mat4 view;
        mat4 projection;
    };

    void main()
    {
        Normal = mat3(transpose(inverse(model))) * aNormal;
        Position = vec3(model * vec4(aPos, 1.0));
        gl_Position = projection * view * vec4(Position, 1.0);
    }
  )";

std::string cmReflectFs = R"(
    #version 430 core
    out vec4 FragColor;

    in vec3 Normal;
    in vec3 Position;

    uniform vec3 cameraPos;
    uniform samplerCube skybox;

    void main()
    {
        vec3 I = normalize(Position - cameraPos);
        vec3 R = reflect(I, normalize(Normal));
        FragColor = vec4(texture(skybox, R).rgb, 1.0);
    }
  )";

std::string cmRefractFs = R"(
    #version 430 core
    out vec4 FragColor;

    in vec3 Normal;
    in vec3 Position;

    uniform float refractiveIndex;
    uniform vec3 cameraPos;
    uniform samplerCube skybox;

    void main()
    {
        float ratio = 1.00 / refractiveIndex;
        vec3 I = normalize(Position - cameraPos);
        vec3 R = refract(I, normalize(Normal), ratio);
        FragColor = vec4(texture(skybox, R).rgb, 1.0);
    }
  )";

std::string visualizeNormalsVS = R"(
#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = view * model * vec4(aPos, 1.0); 
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));
}
)";

std::string visualizeNormalsGS = R"(
#version 430 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.4;
  
uniform mat4 projection;

void GenerateLine(int index)
{
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position + 
                                vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal
} 
)";

std::string visualizeNormalsFS = R"(
#version 430 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}  
)";