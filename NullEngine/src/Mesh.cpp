#include "Mesh.h"

namespace NullEngine
{

Mesh::Mesh(vector<Vertex>&& vertices, vector<unsigned int>&& indices, vector<Texture>&& textures)
{
  this->_vertices = std::move(vertices);
  this->_indices = std::move(indices);
  this->_textures = std::move(textures);

  setupMesh();
}

void Mesh::Draw(Shader& shader)
{
  unsigned int diffuseNr = 1;
  unsigned int specularNr = 1;
  for (unsigned int i = 0; i < _textures.size(); i++)
  {
    glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
    // retrieve texture number (the N in diffuse_textureN)
    std::string number;
    std::string name = _textures[i].Name();
    if (name == "texture_diffuse")
      number = std::to_string(diffuseNr++);
    else if (name == "texture_specular")
      number = std::to_string(specularNr++);

    shader.SetInt(("material." + name + number).c_str(), i);
    glBindTexture(GL_TEXTURE_2D, _textures[i].Id());
  }
  glActiveTexture(GL_TEXTURE0);

  // draw mesh
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, (GLsizei)_indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Mesh::setupMesh()
{
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int),
    &_indices[0], GL_STATIC_DRAW);

  // vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
  // vertex texture coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

  glBindVertexArray(0);
}

}