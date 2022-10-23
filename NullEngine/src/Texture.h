#pragma once
#include <string>

namespace NullEngine
{

class Texture
{
public:
  Texture(const std::string& name, bool flip = false) : _name(name), _flip(flip) {}

  bool Load(const std::string& path, int wrapMode);
  void Use();

private:
  std::string _name;
  bool _flip;
  unsigned int _glId = -1;
  int _width = -1;
  int _height = -1;

};

} // namespace NullEngine
