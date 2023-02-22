#include <glad/glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Engine.h"

namespace NullEngine
{

glm::mat4 myLookAt(NullEngine::Camera& c)
{
  glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
  glm::vec3 camRight = glm::normalize(glm::cross(c._front, worldUp));
  glm::vec3 camUp = glm::normalize(glm::cross(camRight, c._front));

  glm::vec4 rightVec(camRight, 0.0f);
  glm::vec4 upVec(camUp, 0.0f);
  glm::vec4 dirVec(-c._front, 0.0f);
  glm::vec4 lastVec(0.0f, 0.0f, 0.0f, 1.0f);

  glm::mat4 A(rightVec, upVec, dirVec, lastVec);
  A = glm::transpose(A);

  glm::mat4 B = glm::mat4(1.0f);
  B[3] += glm::vec4(-c._pos, 0.0f);

  glm::mat4 lookAt = A * B;
  return lookAt;
}

void Camera::ProcessKeyboard(const GLFWwindow* wnd, float dt)
{
  // camera control
  float cameraSpeed = _movementSpeed * dt;
  float rollSpeed = RollSpeed * dt;

  //glm::vec3 fpsFront(_front.x, 0.0f, _front.z);
  glm::vec3 fpsFront(_front);
  fpsFront = glm::normalize(fpsFront);

  if (glfwGetKey((GLFWwindow*)wnd, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
  {
    cameraSpeed *= _speedBoost;
    rollSpeed *= _rollBoost;
  }

  if (glfwGetKey((GLFWwindow*)wnd, GLFW_KEY_W) == GLFW_PRESS)
    _pos += cameraSpeed * fpsFront;
  if (glfwGetKey((GLFWwindow*)wnd, GLFW_KEY_S) == GLFW_PRESS)
    _pos -= cameraSpeed * fpsFront;
  if (glfwGetKey((GLFWwindow*)wnd, GLFW_KEY_R) == GLFW_PRESS)
    _pos += cameraSpeed * _up;
  if (glfwGetKey((GLFWwindow*)wnd, GLFW_KEY_F) == GLFW_PRESS)
    _pos -= cameraSpeed * _up;
  if (glfwGetKey((GLFWwindow*)wnd, GLFW_KEY_A) == GLFW_PRESS)
    _pos -= cameraSpeed * _right;
  if (glfwGetKey((GLFWwindow*)wnd, GLFW_KEY_D) == GLFW_PRESS)
    _pos += cameraSpeed * _right;

  // ROLL
  if (glfwGetKey((GLFWwindow*)wnd, GLFW_KEY_Q) == GLFW_PRESS)
    _roll = -rollSpeed;

  if (glfwGetKey((GLFWwindow*)wnd, GLFW_KEY_E) == GLFW_PRESS)
    _roll = rollSpeed;

  if (glfwGetKey((GLFWwindow*)wnd, GLFW_KEY_T) == GLFW_PRESS)
  {
    _roll = 0.0f;
    _worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    _up = _worldUp;
  }

  UpdateRoll();
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
  const float cameraSpeed = _movementSpeed * deltaTime;
  const float rollSpeed = RollSpeed * deltaTime;

  // ROLL
  if (direction == Camera_Movement::ROTCW)
    _roll = rollSpeed * _rollBoost;

  if (direction == Camera_Movement::ROTCCW)
    _roll = -rollSpeed * _rollBoost;

  if (direction == Camera_Movement::ROTRESET)
  {
    _worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    _up = _worldUp;
    _roll = 0.0f;
  }
  UpdateRoll();

  glm::vec3 fpsFront(_front);
  fpsFront = glm::normalize(fpsFront);

  if (direction == Camera_Movement::FORWARD)
    _pos += cameraSpeed * _speedBoost * fpsFront;
  if (direction == Camera_Movement::BACKWARD)
    _pos -= cameraSpeed * _speedBoost * fpsFront;
  if (direction == Camera_Movement::LEFT)
    _pos -= cameraSpeed * _speedBoost * _right;
  if (direction == Camera_Movement::RIGHT)
    _pos += cameraSpeed * _speedBoost * _right;
  if (direction == Camera_Movement::UP)
    _pos += cameraSpeed * _speedBoost * _up;
  if (direction == Camera_Movement::DOWN)
    _pos -= cameraSpeed * _speedBoost * _up;
}

void Camera::ProcessMouseMovement(const GLFWwindow* wnd, double xpos, double ypos, bool constrainPitch)
{
  //static float lastX, lastY;
  static bool firstMouse = true;
  if (firstMouse)
  {
    _mouseLastX = (float)xpos;
    _mouseLastY = (float)ypos;
    firstMouse = false;
  }

  // clamp the pitch to avoid LookAt flip
  if (constrainPitch)
  {
    if (_pitch > 89.9f)
    {
      _pitch = 89.9f;
      _pitch2 = 89.9f;
    }
    else if (_pitch < -89.9f)
    {
      _pitch = -89.9f;
      _pitch2 = -89.9f;
    }
  }

  float xoffset = (float)xpos - _mouseLastX;
  float yoffset = _mouseLastY - (float)ypos; // reversed since y-coordinates range from bottom to top
  _mouseLastX = (float)xpos;
  _mouseLastY = (float)ypos;

  xoffset *= _mouseSensitivity;
  yoffset *= _mouseSensitivity;

  _yaw += xoffset;
  _yaw2 = xoffset;
  _pitch += yoffset;
  _pitch2 = yoffset;

  UpdateVectors();
}

void Camera::ProcessMouseMovement(double xpos, double ypos, bool constrainPitch /*= true*/)
{
  static bool firstMouse = true;
  if (firstMouse)
  {
    _mouseLastX = (float)xpos;
    _mouseLastY = (float)ypos;
    firstMouse = false;
  }

  // clamp the pitch to avoid LookAt flip
  if (constrainPitch)
  {
    if (_pitch > 89.9f)
    {
      _pitch = 89.9f;
      //_pitch2 = 89.9f;
    }
    else if (_pitch < -89.9f)
    {
      _pitch = -89.9f;
      //_pitch2 = -89.9f;
    }
  }

  float xoffset = (float)xpos - _mouseLastX;
  float yoffset = _mouseLastY - (float)ypos; // reversed since y-coordinates range from bottom to top
  _mouseLastX = (float)xpos;
  _mouseLastY = (float)ypos;

  xoffset *= _mouseSensitivity;
  yoffset *= _mouseSensitivity;

  _yaw += xoffset;
  _yaw2 = xoffset;
  _pitch += yoffset;
  _pitch2 = yoffset;

  UpdateVectors();
}

void Camera::ProcessMouseScroll(float yoffset, bool boost)
{
  if (boost)
    _movementSpeed += (float)yoffset;
  else
    _movementSpeed += (float)yoffset / 10.0f;

  constexpr float MaxSpeed = 100.0f;
  if (_movementSpeed > MaxSpeed)
    _movementSpeed = MaxSpeed;
  else if(_movementSpeed < 0.0f)
    _movementSpeed = 0.0f;
}

void Camera::ProcessMouseScrollZoom(float yoffset)
{
  _fov -= (float)yoffset;
  if (_fov < FOVmin)
    _fov = FOVmin;
  if (_fov > FOVmax)
    _fov = FOVmax;
}

void Camera::UpdateVectors()
{
  // look direction
  glm::vec3 direction;

  /*direction.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
  direction.y = sin(glm::radians(_pitch));
  direction.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));*/

  glm::mat4 rotYaw(1.0f);
  rotYaw = glm::rotate(rotYaw, glm::radians(-_yaw2), _up);
  direction = glm::vec3(rotYaw * glm::vec4(_front, 0.0f));

  _right = glm::normalize(glm::cross(direction, _worldUp));

  glm::mat4 rotPitch(1.0f);
  rotPitch = glm::rotate(rotPitch, glm::radians(_pitch2), _right);
  direction = glm::vec3(rotPitch * glm::vec4(direction, 0.0f));

  _front = glm::normalize(direction);
  _up = glm::normalize(glm::cross(_right, _front));
}

void Camera::UpdateRoll()
{
  // roll
  if (_roll == 0.0f)
    return;

  glm::mat4 rot(1.0f);
  rot = glm::rotate(rot, glm::radians(_roll), _front);
  //_worldUp = glm::normalize(glm::vec3(rot * glm::vec4(_worldUp, 0.0f)));
  _up = glm::normalize(glm::vec3(rot * glm::vec4(_up, 0.0f)));
  _worldUp = _up;
  _roll = 0.0f;
  UpdateVectors();
}

} // namespace NullEngine