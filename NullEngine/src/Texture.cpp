#include <iostream>
#include "stb/stb_image.h"
#include "Texture.h"

namespace NullEngine
{

bool Texture::Load()
{
  int nrChannels;
  stbi_set_flip_vertically_on_load(_flip);
  unsigned char* data = stbi_load(_path.c_str(), &_width, &_height, &nrChannels, 0);
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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _wrapMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _wrapMode);

  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  if (data)
  {
    glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format, GL_UNSIGNED_BYTE, data);
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

void TextureBase::Use()
{
  glBindTexture(_textureType, _glId);
}

bool CubeMap::Load()
{
  glGenTextures(1, &_glId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, _glId);

  int width, height, nrChannels;
  for (unsigned i = 0; i < _faces.size(); ++i)
  {
    unsigned char* data = stbi_load(_faces[i].c_str(), &width, &height, &nrChannels, 0);
    GLenum format = 0;
    if (nrChannels == 1)
      format = GL_RED;
    else if (nrChannels == 3)
      format = GL_RGB;
    else if (nrChannels == 4)
      format = GL_RGBA;

    if (data)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    }
    else
    {
      std::cout << "Cubemap tex failed to load at path: " << _faces[i] << std::endl;
      stbi_image_free(data);
      return false;
    }
  }
  // wrapMode for CubeMap should be set to GL_CLAMP_TO_EDGE
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, _wrapMode);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, _wrapMode);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, _wrapMode);

  return true;
}

std::vector<std::string> CubeMap::FacesHelper(const std::string &cmName, const std::string &root, const char *imgType)
{
	std::string imgExt;
	if (imgType)
		imgExt = imgType;
	else
		imgExt = ".jpg";

	std::vector<std::string> faces{
			root + cmName + "/right" + imgExt,
			root + cmName + "/left" + imgExt,
			root + cmName + "/top" + imgExt,
			root + cmName + "/bottom" + imgExt,
			root + cmName + "/front" + imgExt,
			root + cmName + "/back" + imgExt};

	return faces;
}

} // namespace NullEngine