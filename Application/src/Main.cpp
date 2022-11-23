#include "IEngine.h"
#include <iostream>

int main()
{
	NullEngine::IEngine *e = (NullEngine::IEngine *)NullEngine::CreateEngine();
	e->Init();

	//e->Run();
	e->Main();

	//LearnOGL();
	LightCasters5_4_main();

	NullEngine::ReleaseEngine(e);
	//std::cin.get();
}