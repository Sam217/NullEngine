#pragma once

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>


class Shader
{
public:
    // the program ID
    unsigned int _ID;

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // Destructor - deletes shader program from openGL
    ~Shader();
    // Use/activate the shader
    void Use() const;
    // utility uniform functions
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetFloat4(const std::string& name, float* value) const;

private:
    // Compile routine
    unsigned CompileShader(unsigned shaderType, const char* shaderSource);
};