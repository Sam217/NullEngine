#pragma once

#ifdef NULLENGINE_BUILD
#define NULLENGINE_API __declspec(dllexport)
#else
#define NULLENGINE_API __declspec(dllimport)
#endif

namespace NullEngine {

	class IEngine
	{
	public:
		//! Dtor
		virtual ~IEngine() = default;
		//! Later initialization
		virtual void Init() = 0;
		//! Main
		virtual int Main() = 0;
	};

	extern "C" NULLENGINE_API void *CreateEngine();
	extern "C" NULLENGINE_API void ReleaseEngine(void *instance);

} // namespace NullEngine

extern "C" NULLENGINE_API int LearnOGL();
extern "C" NULLENGINE_API int LightCasters5_4_main();
extern "C" NULLENGINE_API int Depth_testing_main();
extern "C" NULLENGINE_API int Stencil_testing_main();