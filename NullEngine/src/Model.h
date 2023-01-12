#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"

namespace NullEngine
{

class Model
{
public:
  Model(const char* path, bool flippedTex = false)
  {
    _flippedTextures = flippedTex;
    LoadModel(path);
  }
  void Draw(Shader& shader);

  bool _flippedTextures;

private:
  // model data
  std::vector<Mesh> _meshes;
  std::string _directory;

  void LoadModel(std::string path);
  void ProcessNode(aiNode* node, const aiScene* scene);
  Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
  std::vector<std::shared_ptr<Texture>> LoadMaterialTextures(const aiMaterial* mat, aiTextureType type, const std::string& typeName);
};

}