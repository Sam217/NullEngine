#pragma once
#include <string>

class Texture
{
public:
  Texture(std::string& name, bool flip = false) : _name(name), _flip(flip){}

  bool Load(std::string& path, int mode);
  void Use();

private:
  std::string _name;
  bool _flip;
  unsigned int _glId = -1;
  int _width = -1;
  int _height = -1;

};