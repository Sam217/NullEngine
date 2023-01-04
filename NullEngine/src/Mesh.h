#pragma once

#include <vector>
#include <memory>
#include "Texture.h"
#include "Vertex.h"
#include "Shader.h"

using std::vector;

namespace NullEngine
{

class Mesh
{
public:
  // mesh data
  vector<Vertex>       _vertices;
  vector<unsigned int> _indices;
  vector<std::shared_ptr<Texture>> _textures;

  Mesh(vector<Vertex>&& vertices, vector<unsigned int>&& indices, vector<std::shared_ptr<Texture>>&& textures);
  void Draw(Shader& shader);
private:
  //  render data
  unsigned int VAO, VBO, EBO;

  void setupMesh();
};

}