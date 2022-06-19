#pragma once

#include <string>
#include "IEngine.h"



namespace NullEngine {

	class Engine : IEngine
	{
	public:
		//! Ctor
		Engine() = default;
	private:
		//! Window
		void *_window = nullptr;

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
	};

} // namespace NullEngine
