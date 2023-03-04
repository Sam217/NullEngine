#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "IEngine.h"
#include "Shader.h"
#include "Camera.h"

struct ImGuiIO;

// Basic Phong-shading material
struct Material
{
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;
};

namespace NullEngine {


	class Engine : IEngine
	{
		friend class Camera;
		struct Positions
		{
			std::vector<glm::vec3> cubePositions;
			std::vector<glm::vec3> pointLightPositions;
			glm::vec3 lightPos;
		};

	public:
		static Engine* _engineContext;

		//! Ctor
		Engine() { _camera = Camera(_width / 2.0f, _height / 2.0f); };
	private:
		//! GUI
		ImGuiIO* _io = nullptr;
		bool _captureMouse = true;
		//! Window
		GLFWwindow* _window = nullptr;
		//! Camera
		Camera _camera;

		//! ligts modification params
		int _lightColorIntensity = 100;
		int _lightAmbIntensity = 100;
		int _lightDiffIntensity = 100;
		int _lightSpecIntensity = 100;

		int _spotLightColorIntensity = 100;

		bool _pause = false;
		//
		int _width = 1920;
		//
		int _height = 1080;

		//! list of shaders
		std::vector<std::unique_ptr<Shader>> _shaders;
		//! list of shaders - effects
		std::vector<std::unique_ptr<Shader>> _effects;
		const Shader* _currentEffect = nullptr;
		//!
		std::vector<std::vector<float>> _vertices;
		std::vector<Material> _materials;
		Positions _positions;

	public:
		//! Dtor
		virtual ~Engine() override = default;
		//! Later initialization
		virtual void Init() override;
		//! Main
		virtual int Main() override;
		//! GetGlfwWindow
		GLFWwindow* GetGlfwWindow() { return _window; }
	private:
		//! Init GL Framework
		void InitGLFW();
		//! Create shaders
		void CreateShaders();
		void InitPhongMaterials();
		void InitPositions();
		//!
		void InitVertices();
		void InitImGui();
		//! Process input
		void processInput(float dt);
		void ShowAppDockSpace(bool* p_open);
		//************************************
		// Method:    Framebuffer_size_callback
		// FullName:  NullEngine::Engine::Framebuffer_size_callback
		// Access:    private
		// Returns:   void
		// Qualifier:
		// Parameter: GLFWwindow * window
		// Parameter: int width
		// Parameter: int height
		//************************************
		static void Framebuffer_size_callback(GLFWwindow* window, int width, int height);
		//************************************
		// Method:    Mouse_callback
		// FullName:  NullEngine::Engine::Mouse_callback
		// Access:    private
		// Returns:   void
		// Qualifier:
		// Parameter: GLFWwindow * window
		// Parameter: double xpos
		// Parameter: double ypos
		//************************************
		static void Mouse_callback(GLFWwindow* window, double xpos, double ypos);
		static void Scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	};

} // namespace NullEngine
