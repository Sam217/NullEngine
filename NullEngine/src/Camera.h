#pragma once
#include <glm/glm.hpp>

namespace NullEngine
{

class Camera
{
public:
  //! FOV
  float _fov = 60.0f;

  //! 
  glm::vec3 _pos;
  //! up vec
  glm::vec3 _up;
  //! 
  glm::vec3 _front;


private:
};

} // namespace NullEngine