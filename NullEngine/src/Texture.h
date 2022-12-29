#pragma once
#include <string>
#include <glad/glad.h> 

namespace NullEngine
{

class Texture
{
public:
  Texture() = default;
  Texture(const std::string& name, bool flip = false) : _name(name), _flip(flip) {}

  bool Load(const std::string& path, const std::string& directory, int wrapMode = GL_REPEAT);
  void Use();

  const std::string& Name() const { return _name; }
  void SetName(const std::string& val) { _name = val; }

  const unsigned int& Id() const { return _glId; }
  const std::string& Path() const { return _path; }
  void SetPath(std::string val) { _path = val; }
  //void SetId(unsigned int val) { _glId = val; }

private:
  std::string _name;
  std::string _path;
  bool _flip = false;
  unsigned int _glId = -1;
  int _width = -1;
  int _height = -1;

};

struct STexture
{
  unsigned int id;
  std::string type;
};

} // namespace NullEngine
