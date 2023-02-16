#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "learnopengl_utils/filesystem.h"
#include <learnopengl_utils/shader_m.h>
#include <learnopengl_utils/camera.h>

#include <iostream>
#include "../Engine.h"
#include "../Texture.h"
#include <map>

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void processInput(GLFWwindow* window);
static unsigned int loadTexture(const char* path);

// settings
static const unsigned int SCR_WIDTH = 1920;
static const unsigned int SCR_HEIGHT = 1080;

// camera
//static Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
static NullEngine::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
static float lastX = (float)SCR_WIDTH / 2.0;
static float lastY = (float)SCR_HEIGHT / 2.0;
static bool firstMouse = true;

// timing
static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

static Shader* currentShader;
static Shader* effects[8];

NULLENGINE_API int Depth_testing_main()
{
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);

  // build and compile shaders
  // -------------------------
  std::string shaderRoot = "../LearnOpenGL_guide/shaders/";
  //Shader shader("1.1.depth_testing.vs", "1.1.depth_testing.fs");
  Shader shader((shaderRoot + "1.1.depth_testing.vs").c_str(), (shaderRoot + "1.1.depth_testing.fs").c_str());

  std::string simpleVScode = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoords;

    out vec2 TexCoords;

    void main()
    {
      gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
      TexCoords = aTexCoords;
    })";

  std::string simpleFScode = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoords;
    uniform sampler2D screenTexture;

    void main()
    {
      FragColor = texture(screenTexture, TexCoords);
    })";

  std::string simpleFSnegative = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoords;
    uniform sampler2D screenTexture;

    void main()
    {
      FragColor = vec4(vec3(1.0f - texture(screenTexture, TexCoords)), 1.0);
    })";

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
    })";

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
      })";

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
      })";

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
      })";

  Shader simpleShader(simpleVScode, simpleFScode);
  Shader effectNegative(simpleVScode, simpleFSnegative);
  Shader effectGreyScale(simpleVScode, simpleFSgscale);
  Shader effectGreyScaleWeighted(simpleVScode, simpleFSgscaleW);
  Shader effectSharpen(simpleVScode, fsSharpen);
  Shader effectBlur(simpleVScode, fsBlur);
  Shader effectEdge(simpleVScode, fsEdge);

  effects[0] = &simpleShader;
  effects[1] = &effectNegative;
  effects[2] = &effectGreyScale;
  effects[3] = &effectGreyScaleWeighted;
  effects[4] = &effectSharpen;
  effects[5] = &effectBlur;
  effects[6] = &effectEdge;

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float cubeVertices[] = {
    // positions          // texture Coords
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

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
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
  };
  float planeVertices[] = {
    // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
     5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
    -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,

     5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
     5.0f, -0.5f, -5.0f,  2.0f, 2.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
  };
  float grassQuad[] = {
    // first triangle
    -0.5f,  0.5f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
     0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
    //second triangle
     0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
     0.5f, -0.5f, 0.0f,   1.0f, 0.0f
  };
  // Render target texture quad
  //float quadVertices[] = {
  //  // positions   // texCoords
  //  -1.6f,  0.9f,  0.0f, 1.0f,
  //  -1.6f, -0.9f,  0.0f, 0.0f,
  //   1.6f, -0.9f,  1.0f, 0.0f,

  //  -1.6f,  0.9f,  0.0f, 1.0f,
  //   1.6f, -0.9f,  1.0f, 0.0f,
  //   1.6f,  0.9f,  1.0f, 1.0f
  //};
  float quadVertices[] = {
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };
  // vegetation grass positions
  std::vector<glm::vec3> vegetation;
  vegetation.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
  vegetation.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
  vegetation.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
  vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
  vegetation.push_back(glm::vec3(0.5f, 0.0f, -0.6f));
  // cube VAO
  unsigned int cubeVAO, cubeVBO;
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);
  glBindVertexArray(cubeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glBindVertexArray(0);
  // plane VAO
  unsigned int planeVAO, planeVBO;
  glGenVertexArrays(1, &planeVAO);
  glGenBuffers(1, &planeVBO);
  glBindVertexArray(planeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glBindVertexArray(0);

  // grass VAO
  unsigned grassVAO, grassVBO;
  glGenVertexArrays(1, &grassVAO);
  glGenBuffers(1, &grassVBO);
  glBindVertexArray(grassVAO);
  glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(grassQuad), &grassQuad, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

  // Render target texture quad VAO
  unsigned rtQuadVAO, rtQuadVBO;
  glGenVertexArrays(1, &rtQuadVAO);
  glGenBuffers(1, &rtQuadVBO);
  glBindVertexArray(rtQuadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, rtQuadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  // Render to texture
  unsigned frameBuffer;
  glGenFramebuffers(1, &frameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

  unsigned textureColorBuffer;
  glGenTextures(1, &textureColorBuffer);
  glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  // attach the texture as a color attachment to the framebuffer
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

  // create render buffer object to store depth & stencil buffers for the new framebuffer render target
  unsigned rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  // attach
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);


  // load textures
  // -------------
  std::string root(R"(..\LearnOpenGL_guide\)");
  //unsigned int cubeTexture = loadTexture(FileSystem::getPath(root + "resources/textures/marble.jpg").c_str());
  unsigned int cubeTexture = loadTexture(FileSystem::getPath(root + "../Resources/container.jpg").c_str());
  unsigned int floorTexture = loadTexture(FileSystem::getPath(root + "resources/textures/metal.png").c_str());
  //unsigned int grassTexture2 = loadTexture(FileSystem::getPath(root + "resources/textures/grass.png").c_str());

  NullEngine::Texture grassTexture("grass", true);
  grassTexture.Load("resources/textures/grass.png", root, GL_CLAMP_TO_EDGE);
  NullEngine::Texture redWindow("redWin", true);
  redWindow.Load("resources/textures/blending_transparent_window.png", root, GL_CLAMP_TO_EDGE);

  // shader configuration
  // --------------------
  shader.use();
  shader.setInt("texture1", 0);

  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  currentShader = &simpleShader;
  // render loop
  // -----------
  while (!glfwWindowShouldClose(window))
  {
    // per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // input
    // -----
    processInput(window);

    // render
    // ------
    // first pass
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    // cubes
    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // floor
    glBindVertexArray(planeVAO);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    shader.setMat4("model", glm::mat4(1.0f));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    //const auto& windows = vegetation;
    //std::map<float, glm::vec3> sorted;
    //for (int i = 0; i < windows.size(); ++i)
    //{
    //  float dist = glm::length(windows[i] - camera.Position());
    //  sorted[dist] = windows[i];
    //}
    //glBindVertexArray(grassVAO);
    ////grassTexture.Use();
    //redWindow.Use();
    ////glBindTexture(GL_TEXTURE_2D, grassTexture2);
    //for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
    //{
    //  model = glm::mat4(1.0f);
    //  model = glm::translate(model, it->second);
    //  shader.setMat4("model", model);
    //  glDrawArrays(GL_TRIANGLES, 0, 6);
    //}

    // second pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    currentShader->use();
    glBindVertexArray(rtQuadVAO);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteVertexArrays(1, &planeVAO);
  glDeleteBuffers(1, &cubeVBO);
  glDeleteBuffers(1, &planeVBO);

  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard((NullEngine::Camera::Camera_Movement)FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard((NullEngine::Camera::Camera_Movement)BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard((NullEngine::Camera::Camera_Movement)LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard((NullEngine::Camera::Camera_Movement)RIGHT, deltaTime);
  if (dynamic_cast<NullEngine::Camera*>(&camera))
  {
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
      camera.ProcessKeyboard(NullEngine::Camera::Camera_Movement::UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
      camera.ProcessKeyboard(NullEngine::Camera::Camera_Movement::DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
      camera._speedBoost = 3.0f;
      camera._rollBoost = 2.0f;
    }
    else
    {
      camera._speedBoost = 1.0f;
      camera._rollBoost = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
      camera.ProcessKeyboard(NullEngine::Camera::Camera_Movement::ROTCCW, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
      camera.ProcessKeyboard(NullEngine::Camera::Camera_Movement::ROTCW, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
      camera.ProcessKeyboard(NullEngine::Camera::Camera_Movement::ROTRESET, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS && effects[0])
      currentShader = effects[0];
    if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS && effects[1])
      currentShader = effects[1];
    if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS && effects[2])
      currentShader = effects[2];
    if (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS && effects[3])
      currentShader = effects[3];
    if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS && effects[4])
      currentShader = effects[4];
    if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS && effects[5])
      currentShader = effects[5];
    if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS && effects[6])
      currentShader = effects[6];
    if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS && effects[7])
      currentShader = effects[7];
  }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);
  if (dynamic_cast<NullEngine::Camera*>(&camera))
  {
    camera.ProcessMouseMovement(xposIn, yposIn);
  }
  else
  {
    if (firstMouse)
    {
      lastX = xpos;
      lastY = ypos;
      firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
  }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data)
  {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  }
  else
  {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}