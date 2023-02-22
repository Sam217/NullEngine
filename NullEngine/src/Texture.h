#pragma once
#include <string>
#include <glad/glad.h>
#include <vector>
//#include <glfw3.h>

namespace NullEngine
{

class TextureBase
{
public:
  TextureBase() = default;
  TextureBase(const std::string& name, GLenum internalType, int wrapMode)
    :
    _name(name), _textureType(internalType), _wrapMode(wrapMode) {}
  virtual bool Load() = 0;
  virtual void Use();

  // Setters
  void SetName(const std::string& val) { _name = val; }

  // Getters
  const std::string& Name() const { return _name; }
  const unsigned int& Id() const { return _glId; }

protected:
  std::string _name;
  unsigned int _glId = -1;
  GLenum _textureType;
  GLenum _wrapMode;
};

class Texture : public TextureBase
{
public:
  Texture() = default;
  Texture(const std::string& name, const std::string& path, int wrapMode = GL_REPEAT, bool flip = false)
    :
    TextureBase(name, GL_TEXTURE_2D, wrapMode), _path(path), _flip(flip) {}

  virtual bool Load() override;
  // Getters
  const std::string& Path() const { return _path; }

private:
  bool _flip = false;
  int _width = -1;
  int _height = -1;

  std::string _path;

};

class CubeMap : public TextureBase
{
public:
  CubeMap() = default;
  CubeMap(const std::string& name, const std::vector<std::string>& faces, int wrapMode = GL_CLAMP_TO_EDGE) : TextureBase(name, GL_TEXTURE_CUBE_MAP, wrapMode), _faces(faces) {}

  virtual bool Load() override;
private:
  std::vector<std::string> _faces;
};

struct STexture
{
  unsigned int id;
  std::string type;
};

} // namespace NullEngine
