#include <iostream>
#include <windows.h>
#include <random>
#include <sstream>

#include <glad/glad.h>
#include <glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "Engine.h"
#include "IEngine.h"
#include "Shader.h"
#include "Shaders/ShaderSources.hpp"
#include "Texture.h"
#include "Model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"


void ShowDockingDisabledMessage()
{
  HWND wnd = glfwGetWin32Window(NullEngine::Engine::_engineContext->GetGlfwWindow());
  MessageBoxA(wnd, "Docking not enabled.", "Dockspace disabled!", MB_OK);
}

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
  {
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

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
  static double lastX, lastY;
  static bool mouseSaved = false;
  if (!_engineContext->_captureMouse || _engineContext->_io->WantCaptureMouse)
  {
    glfwSetInputMode(_engineContext->_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    _engineContext->_camera.ResetMouse();
    if (!mouseSaved)
    {
      lastX = xpos;
      lastY = ypos;
    }
    mouseSaved = true;
    return;
  }

  if (mouseSaved)
  {
    //glfwSetCursorPos(window, lastX, lastY);
    mouseSaved = false;
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

  imgExt = ".png";
  std::vector<std::string> faces2
  {
    root + "skybox2/right" + imgExt,
    root + "skybox2/left" + imgExt,
    root + "skybox2/top" + imgExt,
    root + "skybox2/bottom" + imgExt,
    root + "skybox2/front" + imgExt,
    root + "skybox2/back" + imgExt
  };
  CubeMap skyBox2("LearnOpenGLskyBox2", faces2);
  skyBox2.Load();

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

  Shader* objectShader = _shaders[2].get();
  Shader* lightSourceCube = _shaders[3].get();
  Shader* skyBoxShader = _shaders[5].get();
  Shader* cubeMapReflect = _shaders[6].get();

  std::vector<Shader*> activeShaders = {objectShader, lightSourceCube};// _shaders[0].get()};

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

  objectShader->Use();
  objectShader->SetInt("material.diffuse", 0);

  glActiveTexture(GL_TEXTURE0);
  containerDiffuseMap.Use();

  objectShader->SetInt("material.specular", 1);
  glActiveTexture(GL_TEXTURE1);
  containerSpecularMap.Use();

  objectShader->SetInt("material.emissive", 2);
  glActiveTexture(GL_TEXTURE2);
  containerEmissionMap.Use();

  unsigned uboVP;
  glGenBuffers(1, &uboVP);
  glBindBuffer(GL_UNIFORM_BUFFER, uboVP);
  // allocate memory for two float4x4 matrices
  glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);

  glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboVP, 0, 2 * sizeof(glm::mat4));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glm::vec4 clear_color = {0.4f, 0.55f, 0.9f, 0.75f};
  glm::vec4 highlight_color = {0.4f, 0.55f, 0.9f, 0.75f};

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
    static bool showContainers = false;
    static glm::vec3 containersXYZOffset(0.0f);
    static bool highlight = false;
    static float highlightAmount = 0.01f;
    static int shSky_selected = -1;

    // GUI related stuff
    {
      //ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
      static float mouseSensMult = 2.5f;
      static bool showDockSpace = false;

      if (showDockSpace)
        ShowAppDockSpace(&showDockSpace);
      ImGui::Begin("CrappyEngine Settings");
      ImGui::Text("Dear ImGUI DIRTY integration\nPress RIGHT CTRL to show mouse cursor.");
      ImGui::Checkbox("Enable Dockspace", &showDockSpace);
      ImGui::Checkbox("Show Mirror", &showMirror);      // Edit bools storing our window open/close state
      ImGui::SameLine();
      ImGui::Checkbox("Draw containers", &showContainers);
      ImGui::SameLine();
      ImGui::Checkbox("Highlight objects", &highlight);
      static const char* skyBoxes[] = {"black", "SkyBox1", "SkyBox2"};
      ImGui::Combo("Select Skybox", &shSky_selected, skyBoxes, _countof(skyBoxes), 3);

      if (highlight)
      {
        ImGui::SliderFloat("Highlight strength", &highlightAmount, 0.0f, 1.0f);
        ImGui::ColorEdit4("Highlight color", (float*)&highlight_color);
      }

      if (showContainers)
      {
        ImGui::SeparatorText("Containers offset from origin");
        ImGui::SliderFloat3("X | Y | Z", (float*)&containersXYZOffset, -50.0f, 50.0f);
      }

      if (ImGui::CollapsingHeader("Shader Configuration"))
      {
        std::vector<std::pair<std::string, float>> refractiveIds;
        refractiveIds.push_back({"Air", 1.00f});
        refractiveIds.push_back({"Water", 1.33f});
        refractiveIds.push_back({"Ice", 1.309f});
        refractiveIds.push_back({"Glass", 1.52f});
        refractiveIds.push_back({"Diamond", 2.42f});
        //if (ImGui::TreeNode("Configuration##2"))
        {
          ImGui::SeparatorText("Shader options");
          static int shaderObj_current;
          static int shaderCont_current;
          static int shObj_selectedRi = -1;
          static int shCon_selectedRi = -1;

          static const char* itemsObjs[] = {"Phong classic", "CubeMap reflection", "CM-Refraction", "Phong explosion"};
          static const char* itemsContainers[] = {"CubeMap reflection", "Phong classic", "CM-Refraction"};
          ImGui::Combo("Object shader", &shaderObj_current, itemsObjs, _countof(itemsObjs), 2);
          ImGui::SameLine(); HelpMarker(
            "Select which shader to use for objects loaded with assimp lib.");

          auto showRiSelection = [&](int& index)
          {
            if (ImGui::TreeNode("Refractive index selection"))
            {
              for (int i = 0; i < refractiveIds.size(); ++i)
              {
                const auto& ri = refractiveIds[i];
                if (ImGui::Selectable(ri.first.c_str(), index == i))
                  index = i;
              }
              ImGui::TreePop();
            }
          };

          if (shaderObj_current == 2)
            showRiSelection(shObj_selectedRi);

          ImGui::Combo("Containers shader", &shaderCont_current, itemsContainers, _countof(itemsContainers), 2);
          ImGui::SameLine(); HelpMarker(
            "Select which shader to use for simple containers/cubes.");

          if (shaderCont_current == 2)
            showRiSelection(shCon_selectedRi);

          // perform shader setup
          if (shaderObj_current == 0)
          {
            objectShader = _shaders[2].get();
          }
          else if (shaderObj_current == 1)
          {
            objectShader = _shaders[6].get();
          }
          else if (shaderObj_current == 2)
          {
            objectShader = _shaders[7].get();
            objectShader->Use();
            int ri = shObj_selectedRi >= 0 ? shObj_selectedRi : 0;
            objectShader->SetFloat("refractiveIndex", refractiveIds[ri].second);
          }
          else if (shaderObj_current == 3)
          {
            objectShader = _shaders[8].get();
          }

          if (shaderCont_current == 0)
          {
            cubeMapReflect = _shaders[6].get();
          }
          else if (shaderCont_current == 1)
          {
            cubeMapReflect = _shaders[2].get();
          }
          else if (shaderCont_current == 2)
          {
            cubeMapReflect = _shaders[7].get();
            cubeMapReflect->Use();
            int ri = shCon_selectedRi >= 0 ? shCon_selectedRi : 0;
            cubeMapReflect->SetFloat("refractiveIndex", refractiveIds[ri].second);
          }
        }

        //ImGui::TreePop();
      }
      ImGui::SliderFloat("Mouse sensitivity", &mouseSensMult, 0.0f, 10.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
      _camera._mouseSensitivity = mouseSensMult / 100.0f;

      ImGui::Checkbox("Constrain mouse pitch", &_camera._constrainPitch);
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

      glBindBuffer(GL_UNIFORM_BUFFER, uboVP);
      glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
      glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
      glBindBuffer(GL_UNIFORM_BUFFER, 0);

      // draw skybox first
      glDepthMask(GL_FALSE);
      glStencilMask(0);
      skyBoxShader->Use();
      // ... set view and projection matrix
      skyBoxShader->SetMat4("skyBoxView", glm::mat4(glm::mat3(view)));
      // skyBoxShader->SetMat4("projection", projection);
      glBindVertexArray(skyboxVAO);
      if (shSky_selected == 1)
        skyBox.Use();
      else if (shSky_selected == 2)
        skyBox2.Use();
      else
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

      glDrawArrays(GL_TRIANGLES, 0, 36);
      glDepthMask(GL_TRUE);
      glStencilMask(0xFF);
      // ... draw rest of the scene

      // set lighting properties
      lightSourceCube->Use();
      lightSourceCube->SetVec3("lightColor", glm::vec3(1.0f) * (float)_lightColorIntensity / 100.0f);

      // draw light source
      // lightSourceCube->SetMat4("view", view);
      // lightSourceCube->SetMat4("projection", projection);

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
      objectShader->Use();
      // objectShader->SetMat4("view", view);
      // objectShader->SetMat4("projection", projection);

      shaderSingleColor.Use();
      shaderSingleColor.SetMat4("view", view);
      shaderSingleColor.SetMat4("projection", projection);
      objectShader->Use();

      glActiveTexture(GL_TEXTURE0);
      containerDiffuseMap.Use();

      glActiveTexture(GL_TEXTURE1);
      containerSpecularMap.Use();

      glActiveTexture(GL_TEXTURE2);
      containerEmissionMap.Use();

      objectShader->SetVec3("viewPos", cam._pos);

      objectShader->SetVec3("dirLight.ambient", glm::vec3(0.1f)/* * (float)(_lightAmbIntensity  * _lightColorIntensity) / 100.0f / 100.0f*/);
      objectShader->SetVec3("dirLight.diffuse", glm::vec3(0.0f)/* * (float)(_lightDiffIntensity * _lightColorIntensity) / 100.0f / 100.0f*/);
      objectShader->SetVec3("dirLight.specular", glm::vec3(0.0f)/* * (float)(_lightSpecIntensity * _lightColorIntensity) / 100.0f / 100.0f*/);
      objectShader->SetVec3("dirLight.direction", glm::vec3(0.0f, -50.0f, 0.0f));

      //lightShader->SetVec3("pointLight.direction", newLightPos);
      objectShader->SetVec3("pointLight.position", _positions.lightPos);
      // ambient part should not be there
      objectShader->SetVec3("pointLight.ambient", glm::vec3(0.0f) * (float)(_lightAmbIntensity * _lightColorIntensity) / 100.0f / 100.0f);
      objectShader->SetVec3("pointLight.diffuse", glm::vec3(1.0f) * (float)(_lightDiffIntensity * _lightColorIntensity) / 100.0f / 100.0f);
      objectShader->SetVec3("pointLight.specular", glm::vec3(1.0f) * (float)(_lightSpecIntensity * _lightColorIntensity) / 100.0f / 100.0f);


      objectShader->SetFloat("pointLight.constant", 1.0f);
      objectShader->SetFloat("pointLight.linear", 0.09f);
      objectShader->SetFloat("pointLight.quadratic", 0.032f);

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
        objectShader->SetVec3(s.str(), movedPosisitons[i]);

        s.seekp(cur + std::streampos(3));
        s << "diffuse";
        s.put('\0');
        objectShader->SetVec3(s.str(), glm::vec3(1.0f) * (float)(_lightDiffIntensity * _lightColorIntensity) / 100.0f / 100.0f);

        s.seekp(cur + std::streampos(3));
        s << "specular";
        s.put('\0');
        objectShader->SetVec3(s.str(), glm::vec3(1.0f) * (float)(_lightSpecIntensity * _lightColorIntensity) / 100.0f / 100.0f);

        s.seekp(cur + std::streampos(3));
        s << "constant";
        s.put('\0');
        objectShader->SetFloat(s.str(), 1.0f);

        s.seekp(cur + std::streampos(3));
        s << "linear";
        s.put('\0');
        objectShader->SetFloat(s.str(), 0.09f);

        s.seekp(cur + std::streampos(3));
        s << "quadratic";
        s.put('\0');
        objectShader->SetFloat(s.str(), 0.032f);
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

      auto setShaderVars = [&](Shader* sh)
      {
        if (sh->_ID == _shaders[6]->_ID || sh->_ID == _shaders[7]->_ID)
        {
          sh->Use();
          sh->SetVec3("cameraPos", cam._pos);
          // sh->SetMat4("view", view);
          // sh->SetMat4("projection", projection);
        }
        else if (sh->_ID == _shaders[2]->_ID || sh->_ID == _shaders[8]->_ID)
        {
          sh->Use();

          sh->SetVec3("spotLight.ambient", glm::vec3(0.1f) * (float)(_spotLightColorIntensity) / 100.0f);
          sh->SetVec3("spotLight.diffuse", glm::vec3(1.0f) * (float)(_spotLightColorIntensity) / 100.0f);
          sh->SetVec3("spotLight.specular", glm::vec3(1.0f) * (float)(_spotLightColorIntensity) / 100.0f);

          sh->SetVec3("spotLight.position", cam._pos);
          sh->SetVec3("spotLight.direction", cam._front);
          sh->SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
          sh->SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

          sh->SetFloat("spotLight.constant", 1.0f);
          sh->SetFloat("spotLight.linear", 0.09f);
          sh->SetFloat("spotLight.quadratic", 0.032f);

          if (sh->_ID == _shaders[8]->_ID)
          {
            sh->SetFloat("time", frameEnd);
          }
        }
      };

      setShaderVars(objectShader);
      setShaderVars(cubeMapReflect);

      auto drawContainers = [&]()
      {
        cubeMapReflect->Use();
        glBindVertexArray(VAOs[0]);
        for (int i = 0; i < _materials.size(); ++i)
        {
          auto& material = _materials[i];
          //lightShader->SetVec3("material.ambient", material.ambient);
          //lightShader->SetVec3("material.diffuse", material.diffuse);
          //lightShader->SetVec3("material.specular", material.specular);
          //lightShader->SetFloat("material.shininess", material.shininess);

          //lightShader->SetVec3("material.specular", 0.5f, 0.5f, 0.5f);
          //lightShader->SetFloat("material.shininess", 64.0f);

          model = glm::mat4(1.0f);
          model = glm::translate(model, _positions.cubePositions[0] + containersXYZOffset + randvecs[i] + glm::normalize(randvecs[i]) * 2.0f);
          //model = glm::translate(model, cubePositions[i]);
          model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f) * 1.0f);

          angle = rotTime * (-20.0f);
          model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f * sin(time), 0.5f));
          cubeMapReflect->SetMat4("model", model);

          glDrawArrays(GL_TRIANGLES, 0, 36);
        }
      };


      if (showContainers)
        drawContainers();
      objectShader->Use();

      auto drawGuitarBag = [&]()
      {
        model = glm::mat4(1.0);
        glm::vec3 bagPos(0.0f, 5.0f, 1.0f);
        model = glm::translate(model, bagPos);
        //model = glm::rotate(model, glm::degrees(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        objectShader->SetMat4("model", model);
        objectShader->SetFloat("material.shininess", 64.0f);
        guitarBag.Draw(*objectShader);

        if (highlight)
        {
          model = glm::scale(model, glm::vec3(1.0f) + glm::vec3(highlightAmount));
          shaderSingleColor.Use();
          shaderSingleColor.SetVec4("highLightColor", highlight_color);
          shaderSingleColor.SetMat4("model", model);
          guitarBag.Highlight(shaderSingleColor);
        }

      };

      auto drawSingapore = [&]()
      {
        objectShader->Use();
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

        objectShader->SetMat4("model", model);
        /*lightShader->SetVec3("material.ambient", obsidian.ambient);
        lightShader->SetVec3("material.diffuse", obsidian.diffuse);
        lightShader->SetVec3("material.specular", obsidian.specular);
        lightShader->SetFloat("material.shininess", obsidian.shininess);*/

        singapore.Draw(*objectShader);
        if (highlight)
        {
          model = glm::scale(model, glm::vec3(1.0f) + glm::vec3(highlightAmount));
          shaderSingleColor.Use();
          shaderSingleColor.SetVec4("highLightColor", highlight_color);
          shaderSingleColor.SetMat4("model", model);
          singapore.Highlight(shaderSingleColor);
        }
      };
      //destructor.Draw(*lightShader);
      //sponza.Draw(*lightShader);

      drawGuitarBag();
      drawSingapore();
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
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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
  std::unique_ptr<Shader> geomEffect(new Shader((root + "LightingCubeV.glsl").c_str(), (root + "lightingObjectGF.glsl").c_str(), (root + "geometryEffect0.glsl").c_str()));

  std::unique_ptr<Shader> shaderGouraud(new Shader((root + "LightingCubeV_Gouraud.glsl").c_str(), (root + "LightingCubeF_Gouraud.glsl").c_str()));
  /*std::unique_ptr<Shader> shader1 = std::make_unique<Shader>((root + "VertexShader.glsl").c_str(), (root + "FragmentShader.glsl").c_str());
  std::unique_ptr<Shader> shader2 = std::make_unique<Shader>((root + "VertexShader.glsl").c_str(), (root + "FragmentShader2.glsl").c_str());*/

  std::unique_ptr<Shader> simpleShader            = std::make_unique<Shader>(simpleVScode, simpleFScode);
  std::unique_ptr<Shader> effectNegative          = std::make_unique<Shader>(simpleVScode, simpleFSnegative);
  std::unique_ptr<Shader> effectGreyScale         = std::make_unique<Shader>(simpleVScode, simpleFSgscale);
  std::unique_ptr<Shader> effectGreyScaleWeighted = std::make_unique<Shader>(simpleVScode, simpleFSgscaleW);
  std::unique_ptr<Shader> effectSharpen           = std::make_unique<Shader>(simpleVScode, fsSharpen);
  std::unique_ptr<Shader> effectBlur              = std::make_unique<Shader>(simpleVScode, fsBlur);
  std::unique_ptr<Shader> effectEdge              = std::make_unique<Shader>(simpleVScode, fsEdge);

  std::unique_ptr<Shader> skyBoxS                 = std::make_unique<Shader>(skyBoxVSsrc, skyBoxFSsrc);
  std::unique_ptr<Shader> cmReflect               = std::make_unique<Shader>(cmReflectVs, cmReflectFs);
  std::unique_ptr<Shader> cmRefract               = std::make_unique<Shader>(cmReflectVs, cmRefractFs);

  _shaders.push_back(std::move(shader1));
  _shaders.push_back(std::move(shader2));
  _shaders.push_back(std::move(shaderL));
  _shaders.push_back(std::move(shaderLs));
  _shaders.push_back(std::move(shaderGouraud));
  _shaders.push_back(std::move(skyBoxS));
  _shaders.push_back(std::move(cmReflect));
  _shaders.push_back(std::move(cmRefract));
  _shaders.push_back(std::move(geomEffect));

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
    // ZM
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    // ZP
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
    // XM
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    // XP
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    // YM
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
    // YP
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f
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
  const char* glsl_version = "#version 430";
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

void Engine::ShowAppDockSpace(bool* p_open)
{
  // If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
  // In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
  // In this specific demo, we are not using DockSpaceOverViewport() because:
  // - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
  // - we allow the host window to have padding (when opt_padding == true)
  // - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
  // TL;DR; this demo is more complicated than what you would normally use.
  // If we removed all the options we are showcasing, this demo would become:
  //     void ShowExampleAppDockSpace()
  //     {
  //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
  //     }

  static bool opt_fullscreen = false;
  static bool opt_padding = false;
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

  // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
  // because it would be confusing to have two docking targets within each others.
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;// | ImGuiWindowFlags_NoDocking;
  if (opt_fullscreen)
  {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  }
  else
  {
    dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
  }

  // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
  // and handle the pass-thru hole, so we ask Begin() to not render a background.
  if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    window_flags |= ImGuiWindowFlags_NoBackground;

  // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
  // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
  // all active windows docked into it will lose their parent and become undocked.
  // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
  // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
  if (!opt_padding)
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("GUI DockSpace", p_open, window_flags);
  if (!opt_padding)
    ImGui::PopStyleVar();

  if (opt_fullscreen)
    ImGui::PopStyleVar(2);

  // Submit the DockSpace
  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
  {
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
  }
  else
  {
    ShowDockingDisabledMessage();
  }

  if (ImGui::BeginMenuBar())
  {
    if (ImGui::BeginMenu("Options"))
    {
      // Disabling fullscreen would allow the window to be moved to the front of other windows,
      // which we can't undo at the moment without finer window depth/z control.
      ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
      ImGui::MenuItem("Padding", NULL, &opt_padding);
      ImGui::Separator();

      if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
      if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
      if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
      if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
      if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
      ImGui::Separator();

      if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
        *p_open = false;
      ImGui::EndMenu();
    }
    HelpMarker(
      "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!" "\n"
      "- Drag from window title bar or their tab to dock/undock." "\n"
      "- Drag from window menu button (upper-left button) to undock an entire node (all windows)." "\n"
      "- Hold SHIFT to disable docking (if io.ConfigDockingWithShift == false, default)" "\n"
      "- Hold SHIFT to enable docking (if io.ConfigDockingWithShift == true)" "\n"
      "This demo app has nothing to do with enabling docking!" "\n\n"
      "This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window." "\n\n"
      "Read comments in ShowExampleAppDockSpace() for more details.");

    ImGui::EndMenuBar();
  }

  ImGui::End();
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