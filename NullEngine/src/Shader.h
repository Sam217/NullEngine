#pragma once

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>

namespace NullEngine
{

class Shader
{
public:
    // the program ID
    unsigned int _ID;

    // constructor reads and builds the shader from file
    Shader(const char* vertexPath, const char* fragmentPath, const char* geomShPath = nullptr);
    // constructor reads and builds the shader directly from string
    Shader(const std::string& vertexCode, const std::string& fragmentCode, const std::string& geomShCode = "");
    // Destructor - deletes shader program from openGL
    ~Shader();
    // Use/activate the shader
    void Use() const;
    // utility uniform functions
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetFloat4(const std::string& name, float* value) const;

    void SetVec2(const std::string& name, const glm::vec2& value) const;

    void SetVec2(const std::string& name, float x, float y) const;

    void SetVec3(const std::string& name, const glm::vec3& value) const;

    void SetVec3(const std::string& name, float x, float y, float z) const;

    void SetVec4(const std::string& name, const glm::vec4& value) const;

    void SetVec4(const std::string& name, float x, float y, float z, float w) const;

    void SetMat2(const std::string& name, const glm::mat2& mat) const;

    void SetMat3(const std::string& name, const glm::mat3& mat) const;

    void SetMat4(const std::string& name, const glm::mat4& mat) const;

private:
    // Init from std::string
  void InitFromStrings(const std::string& vertexCode, const std::string& fragmentCode, const std::string& geomShCode = "");
    // Compile routine
    unsigned CompileShader(unsigned shaderType, const char* shaderSource);
};

} // namespace NullEngine