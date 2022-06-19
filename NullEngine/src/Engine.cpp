#include <glad/glad.h>
#include <glfw3.h>
#include <iostream>
#include <windows.h>
#include "Engine.h"
#include "IEngine.h"
#include "Shader.h"

namespace NullEngine {

	const char *vertexShaderSource = "#version 330 core\n"
									 "layout (location = 0) in vec3 aPos;\n"
									 "layout (location = 1) in vec3 aColor;\n"
								 	 "out vec4 vertexColor;\n"

									 "void main()\n"
									 "{\n"
									 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
									 "vertexColor = vec4(aColor, 1.0);\n"
									 "}\0";

	const char *fragmentShaderSource = "#version 330 core\n"
									   "out vec4 FragColor;\n"
									   "in vec4 vertexColor;\n"

									   "void main()\n"
									   "{\n"
									   "FragColor = vertexColor;\n"
									   "}\n";

	const char *fragmentShaderSource2 = "#version 330 core\n"
									    "out vec4 FragColor;\n"
									    "uniform vec4 ourColor;\n"
									    
									    "void main()\n"
									    "{\n"
									    "FragColor = ourColor;\n"
									    "}\n";

	void framebuffer_size_callback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void processInput(GLFWwindow *window)
	{
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}

	void Engine::Init()
	{
	}

	int Engine::Main()
	{
		InitGLFW();

		char rawPathName[MAX_PATH];
		//GetModuleFileNameA(NULL, rawPathName, MAX_PATH);
		GetCurrentDirectoryA(MAX_PATH, rawPathName);

		//std::string root = rawPathName + std::string(R"(..\NullEngine\src\)");
		std::string root = R"(..\NullEngine\src\)";
		
		Shader shader1((root + "VertexShader.glsl").c_str(), (root+"FragmentShader.glsl").c_str());
		Shader shader2((root + "VertexShader.glsl").c_str(), (root+"FragmentShader2.glsl").c_str());

	/*	float vertices1[] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f,
		};*/

		float vertices1[] = {
			// positions         // colors
			 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
			-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
			 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
		};

		float vertices2[] = {
			0.1f, 0.5f, 0.0f,
			1.f, 0.5f, 0.0f,
			0.6f, -0.5f, 0.0f
		};

		//float vertices1[] = {
		//	-0.9f, -0.5f, 0.0f, // left
		//	-0.0f, -0.5f, 0.0f, // right
		//	-0.45f, 0.5f, 0.0f, // top
		//};
		//float vertices2[] = {
		//	0.0f, -0.5f, 0.0f, // left
		//	0.9f, -0.5f, 0.0f, // right
		//	0.45f, 0.5f, 0.0f  // top
		//};

		//float vertices[] = {
		//	0.5f, 0.5f, 0.0f,	// top right
		//	0.5f, -0.5f, 0.0f,	// bottom right
		//	-0.5f, -0.5f, 0.0f, // bottom left
		//	-0.5f, 0.5f, 0.0f	// top left
		//};
		//unsigned int indices[] = {
		//	// note that we start from 0!
		//	0, 1, 3, // first triangle
		//	1, 2, 3	 // second triangle
		//}; 

		// Set buffers
		unsigned int VAOs[2];
		glGenVertexArrays(2, VAOs); 

		unsigned int VBOs[2];
		glGenBuffers(2, VBOs);

		unsigned int EBO;
		glGenBuffers(1, &EBO);

		// 1. bind Vertex Array Object
		glBindVertexArray(VAOs[0]);
		// 2. copy our vertices array in a buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

		/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

		// 3. then set our vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
		// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
		glBindVertexArray(0);

		glBindVertexArray(VAOs[1]);

		glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Wireframe or normal drawing mode
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Main loop
		while(!glfwWindowShouldClose((GLFWwindow *)_window)) {
			// process input
			processInput((GLFWwindow *)_window);

			// rendering
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			float time = glfwGetTime();
			// 4. draw the object
			shader1.Use();
			shader1.SetFloat("rotation", time/4.0f);
			shader1.SetFloat("xoffset", (sin(time) / 4.0f));
			glBindVertexArray(VAOs[0]);

			glDrawArrays(GL_TRIANGLES, 0, 3);
			//glDrawArrays(GL_TRIANGLES, 0, 6);

			//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			//glBindVertexArray(0);

			shader2.Use();

			// update the uniform color
			float green = (sin(time) / 2.0f) + 0.5f;
			float greenColor[4] = { 0.0f, green, 0.0f, 1.0f };
			shader2.SetFloat4("ourColor", greenColor);

			glBindVertexArray(VAOs[1]);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			glfwSwapBuffers((GLFWwindow *)_window);
			glfwPollEvents();
		}

		// optional: de-allocate all resources once they've outlived their purpose:
		// ------------------------------------------------------------------------
		glDeleteVertexArrays(2, VAOs);
		glDeleteBuffers(2, VBOs);
		glDeleteBuffers(1, &EBO);

		glfwTerminate();
		return 0;
	}

	void Engine::InitGLFW()
	{
		// Init  GLFW
		std::cout << glfwInit() << std::endl;
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Create window
		_window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
		if(_window == NULL) {
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent((GLFWwindow*)_window);

		// Load GLAD
		if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "Failed to initialize GLAD" << std::endl;
			return;
		}

		int nrAttributes;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
		std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

		// Set Viewport
		glViewport(0, 0, 800, 600);

		// Set callback to call when window is resized
		glfwSetFramebufferSizeCallback((GLFWwindow *)_window, framebuffer_size_callback);

	}

	NULLENGINE_API void *CreateEngine()
	{
		Engine *e = new NullEngine::Engine;
		/*e->Init();
		e->Present();*/
		return e;
	}

	NULLENGINE_API void ReleaseEngine(void *instance)
	{
		std::cout << "Engine was destroyed..." << std::endl;
		delete(NullEngine::Engine *)instance;
	}

} // namespace NullEngine
