#include "IEngine.h"
#include <iostream>

int main()
{
	NullEngine::IEngine *e = (NullEngine::IEngine *)NullEngine::CreateEngine();
	e->Init();

	int choice;
	std::cout << "Select what to run: \n\
		1) NullEngine\n\
		2) LearnOGL()\n\
		3) LightCasters5_4 (learnopengl.com)\n\
		4) Depth testing (learnopengl.com)\n\
		5) Stencil testing (learnopengl.com)" << std::endl;
	std::cin >> choice;

	if (choice == 1)
		e->Main();
  else if (choice == 2)
		LearnOGL();
  else if (choice == 3)
		LightCasters5_4_main();
	else if (choice == 4)
    Depth_testing_main();
  else if (choice == 5)
   Stencil_testing_main();

	NullEngine::ReleaseEngine(e);
	//std::cin.get();
}