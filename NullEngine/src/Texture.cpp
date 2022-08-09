#include <iostream>
#include <glad/glad.h> 
#include "stb/stb_image.h"
#include "Texture.h"

bool Texture::Load(std::string& path, int mode)
{
  std::string root = R"(..\Resources\)";
  int nrChannels;
  stbi_set_flip_vertically_on_load(_flip);
  unsigned char* data = stbi_load((root + _name).c_str(), &_width, &_height, &nrChannels, 0);
  stbi_set_flip_vertically_on_load(false);

  glGenTextures(1, &_glId);
  // setting the texture filtering & wrapping options
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (data)
  {
    glBindTexture(GL_TEXTURE_2D, _glId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, mode, GL_UNSIGNED_BYTE, data);
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
  glBindTexture(GL_TEXTURE_2D, _glId);
}
