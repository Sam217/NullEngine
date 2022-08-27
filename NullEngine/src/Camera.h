#pragma once
#include <glm/glm.hpp>

namespace NullEngine
{

class Camera
{
public:
  Camera() = default;
  Camera(glm::vec3& pos, glm::vec3& up, glm::vec3 front) : _pos(pos), _up(up), _front(front) {}

public:
  //! FOV
  float _fov = 45.0f;

  //! up vec     
  glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 3.0f);
  //! 
  glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
  //! 
  glm::vec3 _front = glm::vec3(0.0f, 0.0f, -1.0f);


private:
};

} // namespace NullEngine