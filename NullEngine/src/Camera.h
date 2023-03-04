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

  // for compatibility with LearnOGLSource
  enum class Camera_Movement
  {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    ROTCW, // clockwise rotation
    ROTCCW, //  counter-clockwise rotation
    ROTRESET //  reset rotation
  };

  Camera() = default;
  Camera(const Camera& other) = default;
  Camera(float lastX, float lastY) : _mouseLastX(lastX), _mouseLastY(lastY) {}
  Camera(const glm::vec3& pos) : _pos(pos) {}
  Camera(const glm::vec3& pos, const glm::vec3& up, const glm::vec3& front, float mouseLastX, float mouseLastY)
   : _pos(pos), _up(up), _worldUp(up), _front(front), _mouseLastX(mouseLastX), _mouseLastY(mouseLastY) {}

private:
  static constexpr float RollSpeed = 50.0f;

public:
  //! Mouse sensitivity
  float _mouseSensitivity = SENSITIVITY;
  bool _constrainPitch = true;
  //! FOV
  float _fov = ZOOM;
  //! Euler angles
  //! pitch (theta)
  float _pitch = PITCH;
  float _pitch2 = PITCH;
  //! yaw (phi)
  float _yaw = YAW;
  float _yaw2 = YAW;
  //! UNUSED
  float _roll = 0.0f;
  float _speedBoost = 3.0f;
  float _rollBoost = 2.0f;

  //! Camera position in world space
  glm::vec3 _pos = glm::vec3(0.0f, 0.0f, 3.0f);
  //! Direction of "UP" in camera space?
  glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
  //! Direction of "UP" in world space
  glm::vec3 _worldUp = glm::vec3(0.0f, 1.0f, 0.0f);;
  //! Direction of camera sight in camera space! -> z is negative because we are facing into the screen (OpenGL system is right-handed, +z aiming from the screen)
  glm::vec3 _front = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 _right = glm::vec3(1.0f, 0.0f, 0.0f);

  //! Get view matrix
  glm::mat4 GetViewMatrix() { return glm::lookAt(_pos, _pos + _front, _up); }
  //! Process keyboard input
  void ProcessKeyboard(const GLFWwindow* wnd, float dt);
  void ProcessKeyboard(Camera_Movement direction, float deltaTime);
  //! Process mouse input
  void ProcessMouseMovement(const GLFWwindow* wnd, double xpos, double ypos);
  void ProcessMouseMovement(double xoffset, double yoffset, bool constrainPitch = true);
  void ProcessMouseScroll(float yoffset, bool boost = false);
  void ProcessMouseScrollZoom(float yoffset);
  inline void ResetMouse() { _resetMouse = true; }
  const glm::vec3& Position() const { return _pos; }
  const float& Zoom() const { return _fov; }

private:
  float _movementSpeed = 2.5f;
  //! last mouse positions
  float _mouseLastX = 0.0f;
  float _mouseLastY = 0.0f;
  bool _resetMouse = true;

  //! Update camera vectors
  void UpdateVectors();
  void UpdateRoll();
};

glm::mat4 myLookAt(Camera& c);

} // namespace NullEngine