#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "IEngine.h"
#include "Shader.h"
#include "Camera.h"



namespace NullEngine {

	class Engine : IEngine
	{
		friend class Camera;

	public:
		static Engine* _engineContext;

		//! Ctor
		Engine() { _camera = Camera(_width / 2.0f, _height / 2.0f); };
	private:
		//! Window
		void *_window = nullptr;
		//! Camera
		Camera _camera;

		//! input for GLSL mix function
		int _ratio = 100;
		int _lightIntensity = 100;
		bool _pause = false;
		//
		int _width = 1600;
		// 
		int _height = 900;

		//! list of shaders
		std::vector<std::unique_ptr<Shader>> _shaders;
		//!
		std::vector<std::vector<float>> _vertices;

	public:
		//! Dtor
		virtual ~Engine() override = default;
		//! Later initialization
		virtual void Init() override;
		//! Main
		virtual int Main() override;

	private:
		//! Init GL Framework
		void InitGLFW();
		//! Create shaders
		void CreateShaders();
		//!
		void InitVertices();
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
		//! Process input
		void processInput();
	};

} // namespace NullEngine
