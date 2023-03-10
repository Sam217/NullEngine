#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include "Shader.h"

namespace NullEngine
{

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned vertex = CompileShader(GL_VERTEX_SHADER, vShaderCode);
    unsigned fragment = CompileShader(GL_FRAGMENT_SHADER, fShaderCode);

    _ID = glCreateProgram();

    int success;
    char infoLog[512];

    glAttachShader(_ID, vertex);
    glAttachShader(_ID, fragment);
    glLinkProgram(_ID);
    // print linking errors if any
    glGetProgramiv(_ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(_ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::Shader(std::string& vertexCode, std::string& fragmentCode)
{
  const char* vShaderCode = vertexCode.c_str();
  const char* fShaderCode = fragmentCode.c_str();

  unsigned vertex = CompileShader(GL_VERTEX_SHADER, vShaderCode);
  unsigned fragment = CompileShader(GL_FRAGMENT_SHADER, fShaderCode);

  _ID = glCreateProgram();

  int success;
  char infoLog[512];

  glAttachShader(_ID, vertex);
  glAttachShader(_ID, fragment);
  glLinkProgram(_ID);
  // print linking errors if any
  glGetProgramiv(_ID, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(_ID, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }

  // delete the shaders as they're linked into our program now and no longer necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

Shader::~Shader()
{
    glDeleteProgram(_ID);
}

void Shader::Use() const
{
    glUseProgram(_ID);
}

void Shader::SetBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(_ID, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(_ID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(_ID, name.c_str()), value);
}

void Shader::SetFloat4(const std::string& name, float* value) const
{
    glUniform4f(glGetUniformLocation(_ID, name.c_str()), value[0], value[1], value[2], value[3]);
}

// ------------------------------------------------------------------------
void Shader::SetVec2(const std::string& name, const glm::vec2& value) const
{
  glUniform2fv(glGetUniformLocation(_ID, name.c_str()), 1, &value[0]);
}
void Shader::SetVec2(const std::string& name, float x, float y) const
{
  glUniform2f(glGetUniformLocation(_ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
  glUniform3fv(glGetUniformLocation(_ID, name.c_str()), 1, &value[0]);
}
void Shader::SetVec3(const std::string& name, float x, float y, float z) const
{
  glUniform3f(glGetUniformLocation(_ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::SetVec4(const std::string& name, const glm::vec4& value) const
{
  glUniform4fv(glGetUniformLocation(_ID, name.c_str()), 1, &value[0]);
}
void Shader::SetVec4(const std::string& name, float x, float y, float z, float w) const
{
  glUniform4f(glGetUniformLocation(_ID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::SetMat2(const std::string& name, const glm::mat2& mat) const
{
  glUniformMatrix2fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::SetMat3(const std::string& name, const glm::mat3& mat) const
{
  glUniformMatrix3fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
{
  glUniformMatrix4fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

unsigned Shader::CompileShader(unsigned shaderType, const char* shaderSource)
{
	// Create vertex shader
	unsigned int shader = glCreateShader(shaderType);

	glShaderSource(shader, 1, &shaderSource, nullptr);
	glCompileShader(shader);

	// Check for successful compilation
	int success = 0;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);

		if (shaderType == GL_VERTEX_SHADER) {
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				<< infoLog << std::endl;
		}
		else if (shaderType == GL_FRAGMENT_SHADER) {

			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				<< infoLog << std::endl;
		}
		else {
			std::cout << "ERROR::SHADER::_________::COMPILATION_FAILED\n"
				<< infoLog << std::endl;
		}

		return -1;
	}

	return shader;
}

} // namespace NullEngine

