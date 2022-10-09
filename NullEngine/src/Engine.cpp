#include <iostream>
#include <windows.h>

#include <glad/glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Engine.h"
#include "IEngine.h"
#include "Shader.h"
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace NullEngine
{

/***************************Engine***************************/

Engine* Engine::_engineContext = nullptr;

void Engine::Framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void Engine::Mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
  _engineContext->_camera.ProcessMouse(window, xpos, ypos);
}

void Engine::Scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  _engineContext->_camera.ProcessMouseScroll(yoffset);
}

void Engine::processInput()
{
  _engineContext = this;
  static float lastFrame = (float)glfwGetTime();

  float currentFrame = (float)glfwGetTime();
  float deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;

  if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose((GLFWwindow*)_window, true);

  if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_UP) == GLFW_PRESS && _ratio < 100)
  {
    ++_ratio;
  }
  if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_DOWN) == GLFW_PRESS && _ratio > 0)
  {
    --_ratio;
  }

  // camera
  _camera.ProcessKeyboard((GLFWwindow*)_window, deltaTime);
}

void Engine::Init()
{
}

int Engine::Main()
{
  InitGLFW();
  CreateShaders();
  InitVertices();

  // obtain resources path
  std::string root = R"(..\Resources\)";
  std::string container_tex_src = "container.jpg";
  std::string face_tex_src("awesomeface.png");
  // texture loading from image

  Texture texture1("container");
  Texture texture2("AwesomeFace", true);
  texture1.Load(root + container_tex_src, GL_RGB, GL_REPEAT);
  texture2.Load(root + face_tex_src, GL_RGBA, GL_REPEAT);

  unsigned int indices[] = {
        0, 1, 2, // first triangle
        3, 0, 2  // second triangle
  };

  auto& lightShader = _shaders[2];
  auto& lightSourceCube = _shaders[3];

  // Set buffers
  unsigned int VAOs[2];
  glGenVertexArrays(2, VAOs);

  unsigned int VBOs[2];
  glGenBuffers(2, VBOs);

  unsigned int EBO[2];
  glGenBuffers(2, EBO);

  // 1. bind Vertex Array Object
  glBindVertexArray(VAOs[0]);
  // 2. copy our vertices array in a buffer for OpenGL to use

  auto& cubeLsource = _vertices[4];
  auto& cubeOb = _vertices[5];

  glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
  glBufferData(GL_ARRAY_BUFFER, cubeOb.size() * sizeof(float), cubeOb.data(), GL_STATIC_DRAW);

  /*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

  // 3. then set our vertex attributes pointers
  //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  /*glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 *sizeof(float)));
  glEnableVertexAttribArray(2);*/

  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
  // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
  glBindVertexArray(0);

  // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  //------------- bind vertex array object for 'light cube'
  glBindVertexArray(VAOs[1]);

  glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
  glBufferData(GL_ARRAY_BUFFER, cubeLsource.size() * sizeof(float), cubeLsource.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  /*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);*/

  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  //-------------------------------

  // Wireframe or normal drawing mode
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  /*glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
  };*/

  glm::vec3 cubePositions[] = {
    glm::vec3(2.0f,  0.0f, -5.0f),
    glm::vec3(-3.0f, 2.0f, 10.0f)
  };

  //glm::vec3 cubePositions[] = {
  //  glm::vec3(0.0f,  0.0f, 0.0f),
  //  glm::vec3(1.2f, 1.0f, 2.0f)
  //};

  auto& lightPos = cubePositions[1];

  _shaders[0]->Use();
  _shaders[0]->SetInt("texture1", 0);
  _shaders[0]->SetInt("texture2", 1);

  std::vector<Shader*> activeShaders = {lightShader.get(), lightSourceCube.get()};// _shaders[0].get()};

  // this enables Z-buffer so that faces overlap correctly when projected to the screen
  glEnable(GL_DEPTH_TEST);
  // Main loop
  while (!glfwWindowShouldClose((GLFWwindow*)_window))
  {
    // process input
    processInput();

    // rendering
    //glClearColor(0.1f, 0.15f, 0.3f, 0.75f);
    glClearColor(0.01f, 0.01f, 0.01f, 0.75f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glClear(GL_COLOR_BUFFER_BIT);

    //glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE0);
    texture1.Use();

    glActiveTexture(GL_TEXTURE1);
    texture2.Use();

    float time = (float)glfwGetTime();
    // 4. draw the object

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

    // note that we're translating the scene in the reverse direction of where we want to move
    glm::mat4 view = glm::mat4(1.0f);
    view = _camera.GetViewMatrix();

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(_camera._fov), float(_width) / float(_height), 0.1f, 100.0f);
    //projection = glm::ortho(-(float)_width / 256, (float)_width / 256, -(float)_height / 256, (float)_height / 256, -100.1f, 100.0f);

    lightShader->Use();
    lightShader->SetVec3("objectColor", 1.0f, 0.5f, 0.31f);
    lightShader->SetVec3("lightColor", 1.0f, 1.0f, 1.0f);
    lightShader->SetVec3("lightPos", lightPos);
    lightShader->SetVec3("viewPos", _camera._pos);

    _shaders[0]->Use();
    _shaders[0]->SetFloat("ratio", _ratio / 100.0f);

    for (unsigned int i = 0; i < 2; i++)
    {
      auto& activeShader = activeShaders[i];
      activeShader->Use();

      activeShader->SetMat4("view", view);
      activeShader->SetMat4("projection", projection);

      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);
      //float angle = 0;// 20.0f * (i + 1);
      float angle = 20.0f * (i + 1);
      if (i % 3 == 0)
        angle = time * angle;

      model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      activeShader->SetMat4("model", model);

      glBindVertexArray(VAOs[i]);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glfwSwapBuffers((GLFWwindow*)_window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(2, VAOs);
  glDeleteBuffers(2, VBOs);
  glDeleteBuffers(2, EBO);

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
  _window = glfwCreateWindow(_width, _height, "LearnOpenGL", NULL, NULL);
  if (_window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return;
  }
  glfwMakeContextCurrent((GLFWwindow*)_window);

  // Load GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return;
  }

  int nrAttributes;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
  std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

  // Set Viewport
  glViewport(0, 0, _width, _height);

  // Set callback to call when window is resized
  glfwSetFramebufferSizeCallback((GLFWwindow*)_window, Framebuffer_size_callback);

  // Set mouse input
  glfwSetInputMode((GLFWwindow*)_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Set mouse callback
  glfwSetCursorPosCallback((GLFWwindow*)_window, Mouse_callback);

  // Set mouse scroll callback
  glfwSetScrollCallback((GLFWwindow*)_window, Scroll_callback);
}

void Engine::CreateShaders()
{
  char rawPathName[MAX_PATH];
  //GetModuleFileNameA(NULL, rawPathName, MAX_PATH);
  GetCurrentDirectoryA(MAX_PATH, rawPathName);

  //std::string root = rawPathName + std::string(R"(..\NullEngine\src\)");
  std::string root = R"(..\NullEngine\src\)";

  std::unique_ptr<Shader> shader1(new Shader((root + "VertexShader.glsl").c_str(), (root + "FragmentShader.glsl").c_str()));
  std::unique_ptr<Shader> shader2(new Shader((root + "VertexShader.glsl").c_str(), (root + "FragmentShader2.glsl").c_str()));
  std::unique_ptr<Shader> shaderL(new Shader((root + "LightingCubeV.glsl").c_str(), (root + "LightingCubeF.glsl").c_str()));
  std::unique_ptr<Shader> shaderLs(new Shader((root + "LightingCubeV.glsl").c_str(), (root + "LightSourceF.glsl").c_str()));

  /*std::unique_ptr<Shader> shader1 = std::make_unique<Shader>((root + "VertexShader.glsl").c_str(), (root + "FragmentShader.glsl").c_str());
  std::unique_ptr<Shader> shader2 = std::make_unique<Shader>((root + "VertexShader.glsl").c_str(), (root + "FragmentShader2.glsl").c_str());*/

  _shaders.push_back(std::move(shader1));
  _shaders.push_back(std::move(shader2));
  _shaders.push_back(std::move(shaderL));
  _shaders.push_back(std::move(shaderLs));
}

void Engine::InitVertices()
{
  // triangle glowing
  std::vector<float> vertices1 = {
    // positions         // colors
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
  };

  //triangle rotating
  std::vector<float> vertices2 = {
    0.1f, 0.5f, 0.0f,
    1.f, 0.5f, 0.0f,
    0.6f, -0.5f, 0.0f
  };

  // rectangle for the container texture
  std::vector<float> verticesa = {
    // positions          // colors           // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
  };

  // rectangle for the awesomeface texture
  std::vector<float> verticesb = {
    // positions          // colors           // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.55f, 0.55f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.45f, 0.55f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  0.45f, 0.45f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,  0.45f, 0.55f,   // top left 
  };

  std::vector<float> baseCubeVertices = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
  };

  std::vector<float> baseCubeVerticesWnormal = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
  };

  _vertices.push_back(verticesa);
  _vertices.push_back(verticesb);
  _vertices.push_back(vertices1);
  _vertices.push_back(vertices2);
  _vertices.push_back(baseCubeVertices);
  _vertices.push_back(baseCubeVerticesWnormal);
}

NULLENGINE_API void* CreateEngine()
{
  Engine* e = new NullEngine::Engine;
  /*e->Init();
  e->Present();*/
  return e;
}

NULLENGINE_API void ReleaseEngine(void* instance)
{
  std::cout << "Engine was destroyed..." << std::endl;
  delete(NullEngine::Engine*)instance;
}

} // namespace NullEngine
