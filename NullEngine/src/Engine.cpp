#include <iostream>
#include <windows.h>
#include <random>
#include <sstream>

#include <glad/glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "Engine.h"
#include "IEngine.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"
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
  // if set, we pass inputs only to ImGUI
  if (!_engineContext->_captureMouse || _engineContext->_io->WantCaptureMouse)
  {
    glfwSetInputMode(_engineContext->_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    return;
  }

  //glfwSetInputMode(_engineContext->_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  _engineContext->_camera.ProcessMouseMovement(window, xpos, ypos);
}

void Engine::Scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  // if set, we pass inputs only to ImGUI
  if (!_engineContext->_captureMouse || _engineContext->_io->WantCaptureMouse)
  {
    glfwSetInputMode(_engineContext->_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    return;
  }

  //glfwSetInputMode(_engineContext->_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    _engineContext->_camera.ProcessMouseScrollZoom((float)yoffset);
  else
  {
    bool boost = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
    _engineContext->_camera.ProcessMouseScroll((float)yoffset, boost);
  }
}

void Engine::processInput(float dt)
{
  _engineContext = this;
  static float leastInterval = 0.0f;
  leastInterval += dt;

  if (glfwGetKey(_window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS && leastInterval > 0.2f)
  {
    assert(_io);
    ImGuiIO& io = *_io;

    // Set mouse input accordingly
    // const int hideCursor = io.WantCaptureMouse ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
    const int hideCursor = _captureMouse ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
    glfwSetInputMode(_window, GLFW_CURSOR, hideCursor);
    OutputDebugStringW(L"RIGHT CONTROL PRESSED\n");
    _captureMouse = !_captureMouse;
    leastInterval = 0.0f;
  }

  // if set, we pass inputs only to ImGUI
  if (_io->WantCaptureKeyboard)
    return;

  if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose((GLFWwindow*)_window, true);

  if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_UP) == GLFW_PRESS)
  {
    if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
      ++_lightAmbIntensity;
    else if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
      ++_lightDiffIntensity;
    else if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
      ++_lightSpecIntensity;
    else
      ++_lightColorIntensity;
  }

  if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_DOWN) == GLFW_PRESS)
  {
    if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
      --_lightAmbIntensity;
    else if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
      --_lightDiffIntensity;
    else if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
      --_lightSpecIntensity;
    else
      --_lightColorIntensity;
  }

  if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_5) == GLFW_PRESS)
  {
    _lightColorIntensity = 100;
    _lightAmbIntensity = 100;
    _lightDiffIntensity = 100;
    _lightSpecIntensity = 100;
  }

  if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
  {
    ++_lightAmbIntensity;
  }

  if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
  {
    --_lightAmbIntensity;
  }

  if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_L) == GLFW_PRESS && leastInterval > 0.2f)
  {
    int* lightIntens;

    if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
      lightIntens = &_spotLightColorIntensity;
    else
      lightIntens = &_lightColorIntensity;

    if (!*lightIntens)
      *lightIntens = 100;
    else
      *lightIntens = 0;

    leastInterval = 0.0f;
  }

  if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_P) == GLFW_PRESS && leastInterval > 0.2f)
  {
    _pause = !_pause;
    leastInterval = 0.0f;
  }

  if (glfwGetKey(_window, GLFW_KEY_KP_0) == GLFW_PRESS && _effects[0].get())
    _currentEffect = _effects[0].get();
  if (glfwGetKey(_window, GLFW_KEY_KP_1) == GLFW_PRESS && _effects[1].get())
    _currentEffect = _effects[1].get();
  if (glfwGetKey(_window, GLFW_KEY_KP_2) == GLFW_PRESS && _effects[2].get())
    _currentEffect = _effects[2].get();
  if (glfwGetKey(_window, GLFW_KEY_KP_3) == GLFW_PRESS && _effects[3].get())
    _currentEffect = _effects[3].get();
  if (glfwGetKey(_window, GLFW_KEY_KP_4) == GLFW_PRESS && _effects[4].get())
    _currentEffect = _effects[4].get();
  if (glfwGetKey(_window, GLFW_KEY_KP_5) == GLFW_PRESS && _effects[5].get())
    _currentEffect = _effects[5].get();
  if (glfwGetKey(_window, GLFW_KEY_KP_6) == GLFW_PRESS && _effects[6].get())
    _currentEffect = _effects[6].get();
  if (glfwGetKey(_window, GLFW_KEY_KP_7) == GLFW_PRESS && _effects[7].get())
    _currentEffect = _effects[7].get();

  // camera
  _camera.ProcessKeyboard((GLFWwindow*)_window, dt);
}

void Engine::Init()
{
}

void Engine::InitPhongMaterials()
{
  // Materials
  Material emerald = {
                      glm::vec3(0.0215f, 0.1745f, 0.0215f),
                      glm::vec3(0.07568f, 0.61424f, 0.07568f),
                      glm::vec3(0.633f, 0.727811f, 0.633f),
                      0.6f * 128
  };
  Material jade = {
                      glm::vec3(0.135f, 0.2225f,	0.1575f),
                      glm::vec3(0.54f,	0.89f,	0.63f),
                      glm::vec3(0.316228f,	0.316228f,	0.316228f),
                      0.1f * 128
  };
  Material obsidian = {
                      glm::vec3(0.05375f,	0.05f,	0.06625f),
                      glm::vec3(0.18275f,	0.17f,	0.22525f),
                      glm::vec3(0.332741f,	0.328634f,	0.346435f),
                      0.3f * 128
  };
  Material pearl = {
                      glm::vec3(0.25f,	0.20725f, 0.20725f),
                      glm::vec3(1.0f,	0.829f,	0.829f),
                      glm::vec3(0.296648f,	0.296648f,	0.296648f),
                      0.088f * 128
  };
  Material copper = {
                      glm::vec3(0.19125f,	0.0735f,	0.0225f),
                      glm::vec3(0.7038f,	0.27048f,	0.0828f),
                      glm::vec3(0.256777f,	0.137622f,	0.086014f),
                      0.1f * 128
  };
  Material gold = {
                      glm::vec3(0.24725f, 0.1995f, 0.0745f),
                      glm::vec3(0.75164f, 0.60648f, 0.22648f),
                      glm::vec3(0.628281f,	0.555802f,	0.366065f),
                      0.4f * 128
  };
  Material chrome = {
                      glm::vec3(0.25f,	0.25f,	0.25f),
                      glm::vec3(0.4f, 0.4f,	0.4f),
                      glm::vec3(0.774597f,	0.774597f,	0.774597f),
                      0.6f * 128
  };
  Material brass = {
                      glm::vec3(0.329412f,	0.223529f,	0.027451f),
                      glm::vec3(0.780392f,	0.568627f,	0.113725f),
                      glm::vec3(0.992157f,	0.941176f,	0.807843f),
                      0.21794872f * 128
  };
  Material bronze = {
                      glm::vec3(0.2125f,	0.1275f,	0.054f),
                      glm::vec3(0.714f,	0.4284f,	0.18144f),
                      glm::vec3(0.393548f,	0.271906f,	0.166721f),
                      0.2f * 128
  };
  Material cyanPlastic = {
                      glm::vec3(0.0f,	0.1f,	0.06f),
                      glm::vec3(0.0f,	0.50980392f,	0.50980392f),
                      glm::vec3(0.50196078f,	0.50196078f,	0.50196078f),
                      0.25f * 128
  };

  _materials = {
    emerald,
    jade,
    obsidian,
    pearl,
    copper,
    gold,
    chrome,
    brass,
    bronze,
    cyanPlastic
  };
}

int Engine::Main()
{
  InitGLFW();
  CreateShaders();
  InitVertices();
  InitImGui();

  InitPositions();
  InitPhongMaterials();

  // obtain resources path
  std::string root = R"(../Resources/)";

  // texture loading from image
  Texture texture1("container", root + "container.jpg");
  Texture texture2("AwesomeFace", root + "awesomeface.png", true);
  texture1.Load();
  texture2.Load();

  Texture containerDiffuseMap("containerWood", root + "container2.png");
  Texture containerSpecularMap("containerSteelBorder", root + "container2_specular.png");
  Texture containerEmissionMap("containerEmission", root + "matrix_container.png");
  containerDiffuseMap.Load();
  containerSpecularMap.Load();
  containerEmissionMap.Load();

  std::string imgExt = ".jpg";
  std::string imgExt2 = ".png";
  std::vector<std::string> faces
  {
    root + "skybox/right" + imgExt,
    root + "skybox/left" + imgExt,
    root + "skybox/top" + imgExt,
    root + "skybox/bottom" + imgExt,
    root + "skybox/front" + imgExt,
    root + "skybox/back" + imgExt
  };
  CubeMap skyBox("LearnOpenGLskyBox", faces);
  skyBox.Load();

  Model guitarBag("../Resources/backpack/backpack.obj", nullptr, true);
  Model singapore("../Resources/singapore/untitled.obj");
  //Model destructor("../Resources/destructor-pesado-imperial-isd-1/Destructor imperial ISD 1.obj");
  //Model sponza("../Resources/sponza/source/sponza.fbx", "../Resources/sponza/textures", false);

  std::string shaderRoot = "../LearnOpenGL_guide/shaders/";
  Shader shaderSingleColor((shaderRoot + "2.stencil_testing.vs").c_str(), (shaderRoot + "2.stencil_single_color.fs").c_str());

  // Set skybox Vertex buffers
  unsigned skyboxVAO, skyboxVBO;
  glGenVertexArrays(1, &skyboxVAO);
  glBindVertexArray(skyboxVAO);
  glGenBuffers(1, &skyboxVBO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  auto& skyboxVertices = _vertices[9];
  glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size() * sizeof(float), skyboxVertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

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
  auto& cubeOb = _vertices[6];

  glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
  glBufferData(GL_ARRAY_BUFFER, cubeOb.size() * sizeof(float), cubeOb.data(), GL_STATIC_DRAW);

  // 3. then set our vertex attributes pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 *sizeof(float)));
  glEnableVertexAttribArray(2);

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

  // Render to texture
  unsigned screenQuadVAO, screenQuadVBO;
  glGenVertexArrays(1, &screenQuadVAO);
  glGenBuffers(1, &screenQuadVBO);
  glBindVertexArray(screenQuadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);

  auto& quadVertices = _vertices[7];
  glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(float), quadVertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  unsigned framebuf;
  glGenFramebuffers(1, &framebuf);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuf);

  const GLsizei Width = _width, Height = _height;
  unsigned textureColor;
  glGenTextures(1, &textureColor);
  glBindTexture(GL_TEXTURE_2D, textureColor);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
  glBindTexture(GL_TEXTURE_2D, 0);

  // attach the color texture to the framebuffer
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColor, 0);

  // Create render buffer object
  unsigned renderBuf;
  glGenRenderbuffers(1, &renderBuf);
  glBindRenderbuffer(GL_RENDERBUFFER, renderBuf);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  // attach render buffer to framebuffer
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuf);

  // framebuffer must be complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "NULLENGINE::ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Render to mirror texture
  unsigned mirrorQuadVAO, mirrorQuadVBO;
  glGenVertexArrays(1, &mirrorQuadVAO);
  glGenBuffers(1, &mirrorQuadVBO);
  glBindVertexArray(mirrorQuadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, mirrorQuadVBO);

  auto& mirrorVertices = _vertices[8];
  glBufferData(GL_ARRAY_BUFFER, mirrorVertices.size() * sizeof(float), mirrorVertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  unsigned mirrorBuf;
  glGenFramebuffers(1, &mirrorBuf);
  glBindFramebuffer(GL_FRAMEBUFFER, mirrorBuf);

  const GLsizei mirrorWidth = _width, mirrorHeight = _height;
  unsigned texMirror;
  glGenTextures(1, &texMirror);
  glBindTexture(GL_TEXTURE_2D, texMirror);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mirrorWidth, mirrorHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texMirror, 0);

  unsigned mirrorRenderBuf;
  glGenRenderbuffers(1, &mirrorRenderBuf);
  glBindRenderbuffer(GL_RENDERBUFFER, mirrorRenderBuf);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mirrorWidth, mirrorHeight);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mirrorRenderBuf);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "NULLENGINE::ERROR::FRAMEBUFFER:: Mirror framebuffer not complete!" << std::endl;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //-------------------------------

  // Wireframe or normal drawing mode
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


  _shaders[0]->Use();
  _shaders[0]->SetInt("texture1", 0);
  _shaders[0]->SetInt("texture2", 1);

  auto& lightShader = _shaders[2];
  auto& lightSourceCube = _shaders[3];
  auto& skyBoxShader = _shaders[5];

  std::vector<Shader*> activeShaders = {lightShader.get(), lightSourceCube.get()};// _shaders[0].get()};

  // this enables Z-buffer so that faces overlap correctly when projected to the screen
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);

  float time = 0.0f, timeLast = 0.0f, deltap = 0.0f;

  // randomize some cube positions
  std::random_device r;
  std::mt19937 gen(r());
  float range = 5.0f;
  std::uniform_real_distribution<float> uniform_dist(-range, range);

  std::vector<glm::vec3> randvecs;
  for (int i = 0; i < _materials.size(); ++i)
  {
    glm::vec3 randvec((float)uniform_dist(gen), (float)uniform_dist(gen), (float)uniform_dist(gen));
    randvecs.push_back(randvec);
  }

  std::uniform_int_distribution<int> uni_sgn(1, 2);
  int randsgn[4];
  std::uniform_real_distribution<float> uni_rad(5.0f, 20.0f);
  int randRadius[4];
  for (int i = 0; i < 4; ++i)
  {
    randsgn[i] = uni_sgn(gen) == 1 ? -1 : 1;
    randRadius[i] = (int)uni_rad(gen);
  }

  lightShader->Use();
  lightShader->SetInt("material.diffuse", 0);

  glActiveTexture(GL_TEXTURE0);
  containerDiffuseMap.Use();

  lightShader->SetInt("material.specular", 1);
  glActiveTexture(GL_TEXTURE1);
  containerSpecularMap.Use();

  lightShader->SetInt("material.emissive", 2);
  glActiveTexture(GL_TEXTURE2);
  containerEmissionMap.Use();

  glm::vec4 clear_color = {0.4f, 0.55f, 0.9f, 0.75f};

  // Time measuring
  float frameBeg = (float)glfwGetTime();
  bool showMirror = false;

  // Main loop
  while (!glfwWindowShouldClose((GLFWwindow*)_window))
  {
    float frameEnd = (float)glfwGetTime();
    // process input
    //glfwPollEvents();
    processInput(frameEnd - frameBeg);
    frameBeg = frameEnd;

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
      static float mouseSensMult = 2.5f;
      ImGui::Begin("CrappyEngine Settings");
      ImGui::Text("Dear ImGUI DIRTY integration\nPress RIGHT CTRL to show mouse cursor.");
      ImGui::Checkbox("Show Mirror", &showMirror);      // Edit bools storing our window open/close state

      ImGui::SliderFloat("Mouse sensitivity", &mouseSensMult, 0.0f, 10.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
      _camera._mouseSensitivity = mouseSensMult / 100.0f;
      ImGui::ColorEdit4("clear color", (float*)&clear_color); // Edit 3 floats representing a color

      /*if (ImGui::Button("Show Mirror"))
        showMirror = !showMirror;
      ImGui::SameLine();
      const char* truestr = "true"; const char* falsestr = "false";
      ImGui::Text("Show mirror = %s", showMirror ? truestr : falsestr);*/

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::End();
    }

    // Rendering
    // 1. pass
    glBindFramebuffer(GL_FRAMEBUFFER, framebuf);
    //glClearColor(0.1f, 0.15f, 0.3f, 0.75f);
    //glClearColor(0.01f, 0.01f, 0.01f, 0.75f);
    glClearColor(clear_color.x* clear_color.w, clear_color.y* clear_color.w, clear_color.z* clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (!_pause)
    {
      time = (float)glfwGetTime() - deltap;
    }
    else
    {
      deltap = (float)glfwGetTime() - time;
    }
    // 4. draw the object
    auto drawScene = [&](Camera& cam, float texWidth, float texHeight)
    {
      // note that we're translating the scene in the reverse direction of where we want to move
      glm::mat4 view = cam.GetViewMatrix();

      glm::mat4 projection = glm::perspective(glm::radians(cam._fov), texWidth / texHeight, 0.1f, 100.0f);
      //projection = glm::ortho(-(float)_width / 256, (float)_width / 256, -(float)_height / 256, (float)_height / 256, -100.1f, 100.0f);

      // draw skybox first
      glDepthMask(GL_FALSE);
      glStencilMask(0);
      skyBoxShader->Use();
      // ... set view and projection matrix
      skyBoxShader->SetMat4("view", glm::mat4(glm::mat3(view)));
      skyBoxShader->SetMat4("projection", projection);
      glBindVertexArray(skyboxVAO);
      skyBox.Use();
      glDrawArrays(GL_TRIANGLES, 0, 36);
      glDepthMask(GL_TRUE);
      glStencilMask(0xFF);
      // ... draw rest of the scene

      // set lighting properties
      lightSourceCube->Use();
      lightSourceCube->SetVec3("lightColor", glm::vec3(1.0f) * (float)_lightColorIntensity / 100.0f);

      // draw light source
      lightSourceCube->SetMat4("view", view);
      lightSourceCube->SetMat4("projection", projection);

      float posTime = time / 3.0f;
      float radius = 5.0f;
      _positions.lightPos = _positions.cubePositions[0] + glm::vec3(radius * cos(posTime) / 2, radius * cos(posTime) / 3, radius * sin(posTime));
      glm::vec3 newLightPos(-0.2f, -1.0f, -0.3f);

      glm::mat4 model(1.0f);
      model = glm::translate(model, _positions.lightPos);
      float angle;
      float rotTime = time / 5.0f;
      angle = -20.0f * 15.5f;

      model = glm::rotate(model, glm::radians(rotTime * angle), glm::vec3(1.0f, 0.3f * sin(time), 0.5f));
      model = glm::scale(model, glm::vec3(1.0f) * 0.2f);
      lightSourceCube->SetMat4("model", model);

      glBindVertexArray(VAOs[1]);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      // draw material cube(s)
      lightShader->Use();
      lightShader->SetMat4("view", view);
      lightShader->SetMat4("projection", projection);

      shaderSingleColor.Use();
      shaderSingleColor.SetMat4("view", view);
      shaderSingleColor.SetMat4("projection", projection);
      lightShader->Use();

      glActiveTexture(GL_TEXTURE0);
      containerDiffuseMap.Use();

      glActiveTexture(GL_TEXTURE1);
      containerSpecularMap.Use();

      glActiveTexture(GL_TEXTURE2);
      containerEmissionMap.Use();

      lightShader->SetVec3("viewPos", cam._pos);

      lightShader->SetVec3("dirLight.ambient", glm::vec3(0.1f)/* * (float)(_lightAmbIntensity  * _lightColorIntensity) / 100.0f / 100.0f*/);
      lightShader->SetVec3("dirLight.diffuse", glm::vec3(0.0f)/* * (float)(_lightDiffIntensity * _lightColorIntensity) / 100.0f / 100.0f*/);
      lightShader->SetVec3("dirLight.specular", glm::vec3(0.0f)/* * (float)(_lightSpecIntensity * _lightColorIntensity) / 100.0f / 100.0f*/);
      lightShader->SetVec3("dirLight.direction", glm::vec3(0.0f, -50.0f, 0.0f));

      //lightShader->SetVec3("pointLight.direction", newLightPos);
      lightShader->SetVec3("pointLight.position", _positions.lightPos);
      // ambient part should not be there
      lightShader->SetVec3("pointLight.ambient", glm::vec3(0.0f) * (float)(_lightAmbIntensity * _lightColorIntensity) / 100.0f / 100.0f);
      lightShader->SetVec3("pointLight.diffuse", glm::vec3(1.0f) * (float)(_lightDiffIntensity * _lightColorIntensity) / 100.0f / 100.0f);
      lightShader->SetVec3("pointLight.specular", glm::vec3(1.0f) * (float)(_lightSpecIntensity * _lightColorIntensity) / 100.0f / 100.0f);


      lightShader->SetFloat("pointLight.constant", 1.0f);
      lightShader->SetFloat("pointLight.linear", 0.09f);
      lightShader->SetFloat("pointLight.quadratic", 0.032f);

      std::stringstream s;
      s << "pointLights[";
      std::streampos cur = s.tellp();

      glm::vec3 movedPosisitons[4];

      for (int i = 0; i < 4; ++i)
      {
        movedPosisitons[i] = _positions.pointLightPositions[i] + glm::vec3(randRadius[i] * cos(randsgn[i] * posTime), randRadius[i] * cos(posTime), randRadius[i] * sin(randsgn[3 - i] * posTime));
        s.seekp(cur);
        s << i << "].position";
        s.put('\0');
        lightShader->SetVec3(s.str(), movedPosisitons[i]);

        s.seekp(cur + std::streampos(3));
        s << "diffuse";
        s.put('\0');
        lightShader->SetVec3(s.str(), glm::vec3(1.0f) * (float)(_lightDiffIntensity * _lightColorIntensity) / 100.0f / 100.0f);

        s.seekp(cur + std::streampos(3));
        s << "specular";
        s.put('\0');
        lightShader->SetVec3(s.str(), glm::vec3(1.0f) * (float)(_lightSpecIntensity * _lightColorIntensity) / 100.0f / 100.0f);

        s.seekp(cur + std::streampos(3));
        s << "constant";
        s.put('\0');
        lightShader->SetFloat(s.str(), 1.0f);

        s.seekp(cur + std::streampos(3));
        s << "linear";
        s.put('\0');
        lightShader->SetFloat(s.str(), 0.09f);

        s.seekp(cur + std::streampos(3));
        s << "quadratic";
        s.put('\0');
        lightShader->SetFloat(s.str(), 0.032f);
      }

      // Draw all point lights
      lightSourceCube->Use();
      for (auto& position : movedPosisitons)
      {
        glm::mat4 model(1.0f);
        model = glm::translate(model, position);
        float angle;
        float rotTime = time / 5.0f;
        angle = -20.0f * 15.5f;

        model = glm::rotate(model, glm::radians(rotTime * angle), glm::vec3(1.0f, 0.3f * sin(time), 0.5f));
        model = glm::scale(model, glm::vec3(1.0f) * 0.2f);
        lightSourceCube->SetMat4("model", model);

        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
      }

      lightShader->Use();

      lightShader->SetVec3("spotLight.ambient", glm::vec3(0.1f) * (float)(_spotLightColorIntensity) / 100.0f);
      lightShader->SetVec3("spotLight.diffuse", glm::vec3(1.0f) * (float)(_spotLightColorIntensity) / 100.0f);
      lightShader->SetVec3("spotLight.specular", glm::vec3(1.0f) * (float)(_spotLightColorIntensity) / 100.0f);

      lightShader->SetVec3("spotLight.position", cam._pos);
      lightShader->SetVec3("spotLight.direction", cam._front);
      lightShader->SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
      lightShader->SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

      lightShader->SetFloat("spotLight.constant", 1.0f);
      lightShader->SetFloat("spotLight.linear", 0.09f);
      lightShader->SetFloat("spotLight.quadratic", 0.032f);

      auto drawContainers = [&]()
      {
        glBindVertexArray(VAOs[0]);
        for (int i = 0; i < _materials.size(); ++i)
        {
          auto& material = _materials[i];
          //lightShader->SetVec3("material.ambient", material.ambient);
          //lightShader->SetVec3("material.diffuse", material.diffuse);
          //lightShader->SetVec3("material.specular", material.specular);
          //lightShader->SetFloat("material.shininess", material.shininess);

          //lightShader->SetVec3("material.specular", 0.5f, 0.5f, 0.5f);
          lightShader->SetFloat("material.shininess", 64.0f);

          /*lightShader->SetVec3("material.ambient", 1.0f, 0.5f, 0.31f);
          lightShader->SetVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
          lightShader->SetVec3("material.specular", 0.5f, 0.5f, 0.5f);
          lightShader->SetFloat("material.shininess", 32.0f);*/
          /*lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
          lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;*/

          /* glm::vec4 lightPosView = view * glm::vec4(lightPos, 1.0f);
           lightShader->SetVec3("lightPos", glm::vec3(lightPosView));*/

           //lightShader->SetVec3("lightPos", lightPos);
           /*glm::vec4 viewview = view * glm::vec4(_camera._pos, 1.0f);
           lightShader->SetVec3("viewPos", glm::vec3(viewview));*/

           //lightShader->SetVec3("lightPos", glm::vec3(view * glm::vec4(newLightPos, 1.0f)));
           //lightShader->SetVec3("light.direction", glm::vec3(view * glm::vec4(newLightPos, 1.0f)));

          model = glm::mat4(1.0f);
          model = glm::translate(model, _positions.cubePositions[0] + randvecs[i] + glm::normalize(randvecs[i]) * 2.0f);
          //model = glm::translate(model, cubePositions[i]);
          model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f) * 1.0f);

          angle = rotTime * (-20.0f);
          model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f * sin(time), 0.5f));
          lightShader->SetMat4("model", model);

          glDrawArrays(GL_TRIANGLES, 0, 36);
        }
      };


      //drawContainers();

      model = glm::mat4(1.0);
      glm::vec3 bagPos(0.0f, 5.0f, 1.0f);
      model = glm::translate(model, bagPos);
      //model = glm::rotate(model, glm::degrees(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

      lightShader->SetMat4("model", model);
      lightShader->SetFloat("material.shininess", 64.0f);
      guitarBag.Draw(*lightShader);

      model = glm::scale(model, glm::vec3(1.1f));
      shaderSingleColor.Use();
      shaderSingleColor.SetMat4("model", model);
      guitarBag.Highlight(shaderSingleColor);

      lightShader->Use();
      model = glm::mat4(1.0);
      glm::vec3 singaporePos(0.0f, -5.0f, 1.0f);
      model = glm::translate(model, singaporePos);
      model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f) * 0.01f);

      /*lightShader->SetVec3("dirLight.ambient", glm::vec3(0.1f));
      lightShader->SetVec3("dirLight.diffuse", glm::vec3(1.0f));
      lightShader->SetVec3("dirLight.specular", glm::vec3(1.0f));

      lightShader->SetVec3("dirLight.direction", glm::vec3(1.0f, 1.0f, 0.0f));

      lightShader->SetFloat("dirLight.constant", 0.0f);
      lightShader->SetFloat("dirLight.linear", 0.0f);
      lightShader->SetFloat("dirLight.quadratic", 0.0f);*/

      lightShader->SetMat4("model", model);
      /*lightShader->SetVec3("material.ambient", obsidian.ambient);
      lightShader->SetVec3("material.diffuse", obsidian.diffuse);
      lightShader->SetVec3("material.specular", obsidian.specular);
      lightShader->SetFloat("material.shininess", obsidian.shininess);*/

      singapore.Draw(*lightShader);
      //destructor.Draw(*lightShader);
      //sponza.Draw(*lightShader);
    };

    drawScene(_camera, float(_width), float(_height));

    // 1.5. pass (render mirror)
    /*_camera._front *= -1;
    _camera._right *= -1;*/
    if (showMirror)
    {
      glBindFramebuffer(GL_FRAMEBUFFER, mirrorBuf);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      Camera mirrorCam(_camera);
      mirrorCam._front *= -1;
      mirrorCam._right *= -1;
      drawScene(mirrorCam, float(_width), float(_height));
      // reset camera to original state
      /*_camera._front *= -1;
      _camera._right *= -1;*/
    }
    // 2.pass = draw the screen (quad)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(.2f, .2f, .6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    _currentEffect->Use();
    glm::mat4 rtTexTransform(1.0f);
    _currentEffect->SetMat4("transform", rtTexTransform);
    glBindVertexArray(screenQuadVAO);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, textureColor);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Draw mirror
    if (showMirror)
    {
      //rtTexTransform = glm::translate(rtTexTransform, glm::vec3(0.5f, -0.5f, 0.0f));
      //rtTexTransform = glm::scale(rtTexTransform, glm::vec3(0.33f));
      _currentEffect->SetMat4("transform", rtTexTransform);
      glBindVertexArray(mirrorQuadVAO);
      glBindTexture(GL_TEXTURE_2D, texMirror);
      glTexImage2D(GL_TEXTURE_2D, 2, GL_RGB, mirrorWidth, mirrorHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

      /*glBindRenderbuffer(GL_RENDERBUFFER, mirrorRenderBuf);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mirrorWidth / 4, mirrorHeight / 4);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);*/

      glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // Rendering GUI
    ImGui::Render();
    int width, height;
    glfwGetFramebufferSize(_window, &width, &height);
    glViewport(0, 0, width, height);
    //glClearColor(clear_color.x* clear_color.w, clear_color.y* clear_color.w, clear_color.z* clear_color.w, clear_color.w);
    //glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    ImGuiIO& io = *_io;
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers((GLFWwindow*)_window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(2, VAOs);
  glDeleteBuffers(2, VBOs);
  glDeleteBuffers(2, EBO);

  glDeleteVertexArrays(1, &screenQuadVAO);
  glDeleteBuffers(1, &screenQuadVBO);

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
  _window = glfwCreateWindow(_width, _height, "OpenGraphicsLearning", NULL, NULL);
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
  std::string root = R"(..\NullEngine\src\Shaders\)";

  std::unique_ptr<Shader> shader1(new Shader((root + "VertexShader.glsl").c_str(), (root + "FragmentShader.glsl").c_str()));
  std::unique_ptr<Shader> shader2(new Shader((root + "VertexShader.glsl").c_str(), (root + "FragmentShader2.glsl").c_str()));
  std::unique_ptr<Shader> shaderL(new Shader((root + "LightingCubeV.glsl").c_str(), (root + "LightingCubeF.glsl").c_str()));
  //std::unique_ptr<Shader> shaderL(new Shader(R"(F:\MEGAsync\source\repos\LearnOpenGL\NullEngine\LearnOpenGL_guide\5.1.light_casters.vs)", R"(F:\MEGAsync\source\repos\LearnOpenGL\NullEngine\LearnOpenGL_guide\5.1.light_casters.fs)"));
  std::unique_ptr<Shader> shaderLs(new Shader((root + "LightingCubeV.glsl").c_str(), (root + "LightSourceF.glsl").c_str()));

  std::unique_ptr<Shader> shaderGouraud(new Shader((root + "LightingCubeV_Gouraud.glsl").c_str(), (root + "LightingCubeF_Gouraud.glsl").c_str()));
  /*std::unique_ptr<Shader> shader1 = std::make_unique<Shader>((root + "VertexShader.glsl").c_str(), (root + "FragmentShader.glsl").c_str());
  std::unique_ptr<Shader> shader2 = std::make_unique<Shader>((root + "VertexShader.glsl").c_str(), (root + "FragmentShader2.glsl").c_str());*/

  std::string simpleVScode = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoords;

    out vec2 TexCoords;
    uniform mat4 transform;

    void main()
    {
      gl_Position = transform * vec4(aPos.x, aPos.y, 0.0, 1.0);
      TexCoords = aTexCoords;
    }
  )";

  std::string simpleFScode = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoords;
    uniform sampler2D screenTexture;

    void main()
    {
      FragColor = texture(screenTexture, TexCoords);
    }
  )";

  std::string simpleFSnegative = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoords;
    uniform sampler2D screenTexture;

    void main()
    {
      FragColor = vec4(vec3(1.0f - texture(screenTexture, TexCoords)), 1.0);
    }
  )";

  std::string simpleFSgscale = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoords;
    uniform sampler2D screenTexture;

    void main()
    {
      vec3 texColor = texture(screenTexture, TexCoords).rgb;
      float avg = (texColor.r + texColor.g + texColor.b) / 3.0f;
      FragColor = vec4(avg, avg, avg, 1.0);
    })";

  std::string simpleFSgscaleW = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoords;
    uniform sampler2D screenTexture;

    void main()
    {
      FragColor = texture(screenTexture, TexCoords);
      float avg = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
      FragColor = vec4(avg, avg, avg, 1.0);
    }
  )";

  std::string fsSharpen = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoords;
    uniform sampler2D screenTexture;

    const float offset = 1.0 / 300.0;

    void main()
    {
      vec2 offsets[9] = vec2[](
          vec2(-offset,  offset), // top-left
          vec2( 0.0f,    offset), // top-center
          vec2( offset,  offset), // top-right
          vec2(-offset,  0.0f),   // center-left
          vec2( 0.0f,    0.0f),   // center-center
          vec2( offset,  0.0f),   // center-right
          vec2(-offset, -offset), // bottom-left
          vec2( 0.0f,   -offset), // bottom-center
          vec2( offset, -offset)  // bottom-right
      );

      float kernel[9] = float[](
          -1, -1, -1,
          -1,  9, -1,
          -1, -1, -1
      );

      vec3 sampleTex[9];
      for(int i = 0; i < 9; i++)
      {
          sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
      }
      vec3 col = vec3(0.0);
      for(int i = 0; i < 9; i++)
          col += sampleTex[i] * kernel[i];

      FragColor = vec4(col, 1.0);
      }
  )";

  std::string fsBlur = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoords;
    uniform sampler2D screenTexture;

    const float offset = 1.0 / 300.0;

    void main()
    {
      vec2 offsets[9] = vec2[](
          vec2(-offset,  offset), // top-left
          vec2( 0.0f,    offset), // top-center
          vec2( offset,  offset), // top-right
          vec2(-offset,  0.0f),   // center-left
          vec2( 0.0f,    0.0f),   // center-center
          vec2( offset,  0.0f),   // center-right
          vec2(-offset, -offset), // bottom-left
          vec2( 0.0f,   -offset), // bottom-center
          vec2( offset, -offset)  // bottom-right
      );

      float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
      );

      vec3 sampleTex[9];
      for(int i = 0; i < 9; i++)
      {
          sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
      }
      vec3 col = vec3(0.0);
      for(int i = 0; i < 9; i++)
          col += sampleTex[i] * kernel[i];

      FragColor = vec4(col, 1.0);
      }
  )";

  std::string fsEdge = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoords;
    uniform sampler2D screenTexture;

    const float offset = 1.0 / 300.0;

    void main()
    {
      vec2 offsets[9] = vec2[](
          vec2(-offset,  offset), // top-left
          vec2( 0.0f,    offset), // top-center
          vec2( offset,  offset), // top-right
          vec2(-offset,  0.0f),   // center-left
          vec2( 0.0f,    0.0f),   // center-center
          vec2( offset,  0.0f),   // center-right
          vec2(-offset, -offset), // bottom-left
          vec2( 0.0f,   -offset), // bottom-center
          vec2( offset, -offset)  // bottom-right
      );

      float kernel[9] = float[](
           1,  1,  1,
           1, -8,  1,
           1,  1,  1
      );

      vec3 sampleTex[9];
      for(int i = 0; i < 9; i++)
      {
          sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
      }
      vec3 col = vec3(0.0);
      for(int i = 0; i < 9; i++)
          col += sampleTex[i] * kernel[i];

      FragColor = vec4(col, 1.0);
      }
  )";

  std::string skyBoxVSsrc = R"(
      #version 330 core
      layout (location = 0) in vec3 aPos;

      out vec3 TexCoords;

      uniform mat4 projection;
      uniform mat4 view;

      void main()
      {
          TexCoords = aPos;
          gl_Position = projection * view * vec4(aPos, 1.0);
      }
  )";

  std::string skyBoxFSsrc = R"(
    #version 330 core
    out vec4 FragColor;

    in vec3 TexCoords;

    uniform samplerCube skybox;

    void main()
    {
        FragColor = texture(skybox, TexCoords);
    }
  )";

  std::unique_ptr<Shader> simpleShader            = std::make_unique<Shader>(simpleVScode, simpleFScode);
  std::unique_ptr<Shader> effectNegative          = std::make_unique<Shader>(simpleVScode, simpleFSnegative);
  std::unique_ptr<Shader> effectGreyScale         = std::make_unique<Shader>(simpleVScode, simpleFSgscale);
  std::unique_ptr<Shader> effectGreyScaleWeighted = std::make_unique<Shader>(simpleVScode, simpleFSgscaleW);
  std::unique_ptr<Shader> effectSharpen           = std::make_unique<Shader>(simpleVScode, fsSharpen);
  std::unique_ptr<Shader> effectBlur              = std::make_unique<Shader>(simpleVScode, fsBlur);
  std::unique_ptr<Shader> effectEdge              = std::make_unique<Shader>(simpleVScode, fsEdge);

  std::unique_ptr<Shader> skyBoxS                 = std::make_unique<Shader>(skyBoxVSsrc, skyBoxFSsrc);

  _shaders.push_back(std::move(shader1));
  _shaders.push_back(std::move(shader2));
  _shaders.push_back(std::move(shaderL));
  _shaders.push_back(std::move(shaderLs));
  _shaders.push_back(std::move(shaderGouraud));
  _shaders.push_back(std::move(skyBoxS));

  // simple shader to render screen quad (idx = 5)
  _effects.push_back(std::move(simpleShader));
  _currentEffect = _effects[0].get();

  // effects
  _effects.push_back(std::move(effectNegative));
  _effects.push_back(std::move(effectGreyScale));
  _effects.push_back(std::move(effectGreyScaleWeighted));
  _effects.push_back(std::move(effectSharpen));
  _effects.push_back(std::move(effectBlur));
  _effects.push_back(std::move(effectEdge));

  // DIRTY solution
  _effects.resize(8);
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

  std::vector<float> cubeVertNormalTex = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
  };

  std::vector<float> screenQuad = {
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };

  //std::vector<float> mirrorQuad = {
  //  // positions   // texCoords
  //  -1.0f,  1.0f,  0.0f, 1.0f,
  //  -1.0f,  0.5f,  0.0f, 0.0f,
  //  -0.5f,  0.5f,  1.0f, 0.0f,

  //  -1.0f,  1.0f,  0.0f, 1.0f,
  //  -0.5f,  0.5f,  1.0f, 0.0f,
  //  -0.5f,  1.0f,  1.0f, 1.0f
  //};

  // actually mirrored texcoords
  std::vector<float> mirrorQuad = {
    // positions   // texCoords
    -1.0f,  1.0f,  1.0f, 1.0f,
    -1.0f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.0f, 0.0f,

    -1.0f,  1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  1.0f,  0.0f, 1.0f
  };

  std::vector<float> skyboxVertices = {
    // positions
    // all NEG Z -> Z- side (back)
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    // all NEG -> X- side (left)
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    // all POS X -> X+ side (right)
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     // all POS Z -> Z+ side (front)
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    // all POS Y -> Y+ side (top)
    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    // all NEG Y -> Y- side (bottom)
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
  };

  _vertices.push_back(std::move(verticesa));
  _vertices.push_back(std::move(verticesb));
  _vertices.push_back(std::move(vertices1));
  _vertices.push_back(std::move(vertices2));
  _vertices.push_back(std::move(baseCubeVertices));
  _vertices.push_back(std::move(baseCubeVerticesWnormal));
  _vertices.push_back(std::move(cubeVertNormalTex));
  _vertices.push_back(std::move(screenQuad));
  _vertices.push_back(std::move(mirrorQuad));
  _vertices.push_back(std::move(skyboxVertices));
}

void Engine::InitImGui()
{
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  _io = &ImGui::GetIO(); //(void)io;
  ImGuiIO& io = *_io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
  //io.ConfigViewportsNoAutoMerge = true;
  //io.ConfigViewportsNoTaskBarIcon = true;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsLight();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  // Setup Platform/Renderer backends
  const char* glsl_version = "#version 130";
  ImGui_ImplGlfw_InitForOpenGL(_window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
  // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
  // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
  // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
  //io.Fonts->AddFontDefault();
  //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  io.Fonts->AddFontFromFileTTF("../Dependencies/imgui/misc/fonts/Roboto-Medium.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
  //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
  //IM_ASSERT(font != NULL);

  // Our state
  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

void Engine::InitPositions()
{
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

  /*glm::vec3 cubePositions[] = {
    glm::vec3(2.0f,  0.0f, -5.0f),
    glm::vec3(-3.0f, 2.0f, 10.0f)
  };*/

  _positions.cubePositions = {
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
  };

  //glm::vec3 cubePositions[] = {
  //  glm::vec3(0.0f,  0.0f, 0.0f),
  //  glm::vec3(1.2f, 1.0f, 2.0f)
  //};

  _positions.pointLightPositions = {
  glm::vec3(0.7f,  0.2f,  2.0f),
  glm::vec3(2.3f, -3.3f, -4.0f),
  glm::vec3(-4.0f,  2.0f, -12.0f),
  glm::vec3(0.0f,  0.0f, -3.0f)
  };

  //auto& lightPos = cubePositions[1];
  _positions.lightPos = {1.2f, 1.0f, 2.0f};
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
