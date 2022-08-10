#pragma once

#include <string>
#include <vector>
#include "IEngine.h"
#include "Shader.h"



namespace NullEngine {

	class Engine : IEngine
	{
	public:
		//! Ctor
		Engine() = default;
	private:
		//! Window
		void *_window = nullptr;
		//! input for GLSL mix function
		int _ratio = 2;
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
		//! Process input
		void processInput();
	};

} // namespace NullEngine
