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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void Engine::processInput()
{
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

  if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_W) == GLFW_PRESS)
  {
    _zoffset += 0.1;
  }

  if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_S) == GLFW_PRESS)
  {
    _zoffset -= 0.1;
  }

  if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_R) == GLFW_PRESS)
  {
    _yoffset -= 0.1;
  }

  if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_F) == GLFW_PRESS)
  {
    _yoffset += 0.1;
  }

  if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_A) == GLFW_PRESS)
  {
    _xoffset += 0.1;
  }

  if (glfwGetKey((GLFWwindow*)_window, GLFW_KEY_D) == GLFW_PRESS)
  {
    _xoffset -= 0.1;
  }
}

/***************************Engine***************************/

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
  //int width, height, nrChannels;
  //unsigned char* data = stbi_load((root + container_tex_src).c_str(), &width, &height, &nrChannels, 0);

  //unsigned int texture;
  //glGenTextures(1, &texture);
  //// setting the texture filtering & wrapping options
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  //if (data)
  //{
  //  glBindTexture(GL_TEXTURE_2D, texture);
  //  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  //  glGenerateMipmap(GL_TEXTURE_2D);

  //  glBindTexture(GL_TEXTURE_2D, 0);
  //}
  //else
  //{
  //  std::cout << "Texture failed to load!" << std::endl;
  //}

  //stbi_image_free(data);

  Texture texture1(container_tex_src);
  Texture texture2(face_tex_src, true);
  texture1.Load(root, GL_RGB, GL_REPEAT);
  texture2.Load(root, GL_RGBA, GL_REPEAT);

  unsigned int indices[] = {
        0, 1, 2, // first triangle
        3, 0, 2  // second triangle
  };

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
  glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
  glBufferData(GL_ARRAY_BUFFER, _vertices[4].size() * sizeof(float), _vertices[4].data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // 3. then set our vertex attributes pointers
  //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
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

  ////------------- bind vertex array object for awesomoeface
  //glBindVertexArray(VAOs[1]);

  //glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
  //glBufferData(GL_ARRAY_BUFFER, _vertices[1].size() * sizeof(float), _vertices[1].data(), GL_STATIC_DRAW);

  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
  //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  //glEnableVertexAttribArray(0);
  //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  //glEnableVertexAttribArray(1);
  //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  //glEnableVertexAttribArray(2);

  //// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  //glBindBuffer(GL_ARRAY_BUFFER, 0);

  //// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
  //// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
  //glBindVertexArray(0);
  //// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  //-------------------------------

  // Wireframe or normal drawing mode
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  _shaders[0]->Use();
  _shaders[0]->SetInt("texture1", 0);
  _shaders[0]->SetInt("texture2", 1);

  /*trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0, 1.0));
  trans = glm::scale(trans, glm::vec3(.5, .5, .5));*/
  glm::vec3 cubePositions[] = {
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

  glEnable(GL_DEPTH_TEST);
  // Main loop
  while (!glfwWindowShouldClose((GLFWwindow*)_window))
  {
    // process input
    processInput();

    // rendering
    glClearColor(0.1f, 0.15f, 0.3f, 0.75f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glClear(GL_COLOR_BUFFER_BIT);

    //glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE0);
    texture1.Use();

    glActiveTexture(GL_TEXTURE1);
    texture2.Use();

    float time = glfwGetTime();
    // 4. draw the object
    _shaders[0]->Use();
    /*_shaders[0]->SetFloat("rotation", time / 4.0f);
    _shaders[0]->SetFloat("xoffset", (sin(time) / 4.0f));*/
    _shaders[0]->SetFloat("ratio", _ratio / 100.0f);

    /*glm::mat4 trans = glm::mat4(1.0);
    trans = glm::rotate(trans, time / 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));*/

    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

    glm::mat4 view = glm::mat4(1.0f);
    // note that we're translating the scene in the reverse direction of where we want to move
    view = glm::translate(view, glm::vec3(_xoffset, _yoffset, -3.0f + _zoffset));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), float(_width) / float(_height), 0.1f, 100.0f);

    //_shaders[0]->SetMat4("model", model);
    _shaders[0]->SetMat4("view", view);
    _shaders[0]->SetMat4("projection", projection);

    glBindVertexArray(VAOs[0]);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_TRIANGLES, 0, 36);
    //glDrawArrays(GL_TRIANGLES, 0, 6);

    for (unsigned int i = 0; i < 10; i++)
    {
      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, cubePositions[i]);
      float angle = 20.0f * (i + 1);
      if (i % 3 == 0)
        angle = time * angle;

      model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      _shaders[0]->SetMat4("model", model);

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
  glfwSetFramebufferSizeCallback((GLFWwindow*)_window, framebuffer_size_callback);

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

  /*std::unique_ptr<Shader> shader1 = std::make_unique<Shader>((root + "VertexShader.glsl").c_str(), (root + "FragmentShader.glsl").c_str());
  std::unique_ptr<Shader> shader2 = std::make_unique<Shader>((root + "VertexShader.glsl").c_str(), (root + "FragmentShader2.glsl").c_str());*/

  _shaders.push_back(std::move(shader1));
  _shaders.push_back(std::move(shader2));
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
    - 0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,  0.45f, 0.45f,   // bottom left
    - 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,  0.45f, 0.55f,   // top left 
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

  _vertices.push_back(verticesa);
  _vertices.push_back(verticesb);
  _vertices.push_back(vertices1);
  _vertices.push_back(vertices2);
  _vertices.push_back(baseCubeVertices);
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
