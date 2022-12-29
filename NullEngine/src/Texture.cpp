#include <iostream>
#include "stb/stb_image.h"
#include "Texture.h"

namespace NullEngine
{

bool Texture::Load(const std::string& path, const std::string& directory, int wrapMode /* = GL_REPEAT*/)
{
  int nrChannels;
  stbi_set_flip_vertically_on_load(_flip);
  unsigned char* data = stbi_load((directory + path).c_str(), &_width, &_height, &nrChannels, 0);
  stbi_set_flip_vertically_on_load(false);

  GLenum format = 0;
  if (nrChannels == 1)
    format = GL_RED;
  else if (nrChannels == 3)
    format = GL_RGB;
  else if (nrChannels == 4)
    format = GL_RGBA;

  glGenTextures(1, &_glId);
  glBindTexture(GL_TEXTURE_2D, _glId);
  // setting the texture filtering & wrapping options
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
 // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (data)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    return true;
  }
  else
  {
    std::cout << "Texture failed to load!" << std::endl;

    stbi_image_free(data);
    return false;
  }

}

void Texture::Use()
{
  glBindTexture(GL_TEXTURE_2D, Id());
}

} // namespace NullEngine