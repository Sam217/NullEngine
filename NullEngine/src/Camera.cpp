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
  float cameraSpeed = 2.5f * dt;

  //glm::vec3 fpsFront(_front.x, 0.0f, _front.z);
  glm::vec3 fpsFront(_front);
  fpsFront = glm::normalize(fpsFront);

  if (glfwGetKey((GLFWwindow*)wnd, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
  {
    cameraSpeed *= 3.0f;
  }

  if (glfwGetKey((GLFWwindow*)wnd, GLFW_KEY_W) == GLFW_PRESS)
  {
    //_camera._pos += cameraSpeed * _camera._front;
    _pos += cameraSpeed * fpsFront;
  }

  if (glfwGetKey((GLFWwindow*)wnd, GLFW_KEY_S) == GLFW_PRESS)
  {
    //_camera._pos -= cameraSpeed * _camera._front;
    _pos -= cameraSpeed * fpsFront;
  }

  if (glfwGetKey((GLFWwindow*)wnd, GLFW_KEY_R) == GLFW_PRESS)
  {
    _pos += cameraSpeed * _up;
  }

  if (glfwGetKey((GLFWwindow*)wnd, GLFW_KEY_F) == GLFW_PRESS)
  {
    _pos -= cameraSpeed * _up;
  }

  if (glfwGetKey((GLFWwindow*)wnd, GLFW_KEY_A) == GLFW_PRESS)
  {
    //_camera._pos += cameraSpeed * glm::normalize(glm::cross(_camera._up, _camera._front));
    _pos += cameraSpeed * glm::normalize(glm::cross(_up, fpsFront));
  }

  if (glfwGetKey((GLFWwindow*)wnd, GLFW_KEY_D) == GLFW_PRESS)
  {
    //_camera._pos -= cameraSpeed * glm::normalize(glm::cross(_camera._up, _camera._front));
    _pos -= cameraSpeed * glm::normalize(glm::cross(_up, fpsFront));
  }
}

void Camera::ProcessMouse(const GLFWwindow* wnd, double xpos, double ypos, bool constrainPitch)
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
    if (_pitch > 89)
      _pitch = 89;
    else if (_pitch < -89)
      _pitch = -89;
  }

  float xoffset = (float)xpos - _mouseLastX;
  float yoffset = _mouseLastY - (float)ypos; // reversed since y-coordinates range from bottom to top
  _mouseLastX = (float)xpos;
  _mouseLastY = (float)ypos;

  xoffset *= _mouseSensitivity;
  yoffset *= _mouseSensitivity;

  _yaw += xoffset;
  _pitch += yoffset;

  UpdateVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
  _fov -= (float)yoffset;
  if (_fov <FOVmin)
    _fov = FOVmin;
  if (_fov > FOVmax)
    _fov = FOVmax;
}

void Camera::UpdateVectors()
{
  glm::vec3 direction;
  direction.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
  direction.y = sin(glm::radians(_pitch));
  direction.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));

  _front = glm::normalize(direction);
}

} // namespace NullEngine