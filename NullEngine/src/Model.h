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
  //! Empty constructor
	Model() = default;
  //! Initializing constructor
  Model(const char* path, const char* texturesPath = nullptr, bool flippedTex = false)
  {
    _flippedTextures = flippedTex;
    if (texturesPath)
      _texturesDirectory = texturesPath;
    LoadModel(path);
  }

  void Draw(Shader& shader);
  void Highlight(Shader& shader);

  bool _flippedTextures;

private:
  // model data
  std::vector<Mesh> _meshes;
  std::string _directory;
  std::string _texturesDirectory;

  void LoadModel(std::string path);
  void ProcessNode(aiNode* node, const aiScene* scene);
  Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
  std::vector<std::shared_ptr<Texture>> LoadMaterialTextures(const aiMaterial* mat, aiTextureType type, const std::string& typeName);
};

}