#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//! Forward declaration of GLFWwindow
struct GLFWwindow;

namespace NullEngine
{

// Default camera values
constexpr float YAW = -90.0f;
constexpr float PITCH = 0.0f;
constexpr float SPEED = 2.5f;
constexpr float SENSITIVITY = 0.025f;
constexpr float ZOOM = 45.0f;

static constexpr float FOVmax = 125.0f;
static constexpr float FOVmin = 1.0f;

//! Engine class forward declaration - needed to avoid circular include hell
class Engine;

class Camera
{
public:
  // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
  enum class CameraDir
  {
    Forward,
    Backward,
    Left,
    Right
  };

  Camera() = default;
  Camera(const Camera& other) = default;
  Camera(float lastX, float lastY) : _mouseLastX(lastX), _mouseLastY(lastY) {};
  Camera(glm::vec3& pos, glm::vec3& up, glm::vec3 front, float mouseLastX, float mouseLastY) 
   : _pos(pos), _up(up), _worldUp(up), _front(front), _mouseLastX(mouseLastX), _mouseLastY(mouseLastY) {}

public:
  //! Mouse sensitivity
  float _mouseSensitivity = SENSITIVITY;
  //! FOV
  float _fov = ZOOM;
  //! Euler angles
  //! pitch
  float _pitch = PITCH;
  //! yaw
  float _yaw = YAW;
  //! UNUSED
  float _roll = 0.0f;

  //! up vec     
  glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 3.0f);
  //! 
  glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 _worldUp = glm::vec3(0.0f, 1.0f, 0.0f);;
  //! 
  glm::vec3 _front = glm::vec3(0.0f, 0.0f, -1.0f);

  //! Get view matrix
  glm::mat4 GetViewMatrix() { return glm::lookAt(_pos, _pos + _front, _up); }
  //! Process keyboard input
  void ProcessKeyboard(const GLFWwindow* wnd, float dt);
  //! Process mouse input
  void ProcessMouseMovement(const GLFWwindow* wnd, double xpos, double ypos, bool constrainPitch = true);
  void ProcessMouseScroll(float yoffset);

private:
  //! last mouse positions
  float _mouseLastX = 0.0f;
  float _mouseLastY = 0.0f;

  //! Update camera vectors
  void UpdateVectors();
  void UpdateRoll();
};

glm::mat4 myLookAt(Camera& c);

} // namespace NullEngine