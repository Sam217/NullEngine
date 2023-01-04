#include <iostream>
#include "Model.h"
#include <set>
#include <map>

namespace NullEngine
{
  
void Model::Draw(Shader& shader)
{
  for (unsigned i = 0; i < _meshes.size(); i++)
    _meshes[i].Draw(shader);
}

void Model::LoadModel(std::string path)
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    return;
  }
  _directory = path.substr(0, path.find_last_of('/'));

  ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
  // process all the node's meshes (if any)
  for (unsigned i = 0; i < node->mNumMeshes; ++i)
  {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    _meshes.push_back(ProcessMesh(mesh, scene));
  }
  // then do the same for each of its children
  for (unsigned i = 0; i < node->mNumChildren; ++i)
  {
    ProcessNode(node->mChildren[i], scene);
  }
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<std::shared_ptr<Texture>> textures;

  for (unsigned i = 0; i < mesh->mNumVertices; i++)
  {
    Vertex vertex;
    // process vertex positions, normals and texture coordinates
    const aiVector3D& meshVec = mesh->mVertices[i];
    vertex.Position = glm::vec3(meshVec.x, meshVec.y, meshVec.z);

    const aiVector3D& meshVecN = mesh->mNormals[i];
    vertex.Normal = glm::vec3(meshVecN.x, meshVecN.y, meshVecN.z);

    if (mesh->mTextureCoords[0])
    {
      const auto& texCoords = mesh->mTextureCoords[0][i];
      vertex.TexCoords = glm::vec2(texCoords.x, texCoords.y);
    }
    else
      vertex.TexCoords = glm::vec2(0.0f);

    vertices.push_back(vertex);
  }
  // process indices
  for (unsigned i = 0; i < mesh->mNumFaces; ++i)
  {
    const aiFace& face = mesh->mFaces[i];
    for (int j = 0; i < face.mNumIndices; ++j)
      indices.push_back(face.mIndices[j]);
  }
  // process material
  if (mesh->mMaterialIndex >= 0)
  {
    const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // add diffuse maps
    textures = LoadMaterialTextures(material, aiTextureType_DIFFUSE, std::string("texture_diffuse"));
    // add specular maps
    std::vector<std::shared_ptr<Texture>> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, std::string("texture_specular"));
    textures.insert(textures.end(), std::make_move_iterator(specularMaps.begin()), std::make_move_iterator(specularMaps.end()));
  }

  return Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

std::map<std::string, std::shared_ptr<Texture>> loaded_textures;

std::vector<std::shared_ptr<Texture>> Model::LoadMaterialTextures(const aiMaterial* mat, aiTextureType type, const std::string& typeName)
{
  std::vector<std::shared_ptr<Texture>> textures;
  for (unsigned i = 0; i < mat->GetTextureCount(type); ++i)
  {
    std::shared_ptr<Texture> tex = std::make_shared<Texture>();
    aiString aipath;
    mat->GetTexture(type, i, &aipath);

    auto loaded = loaded_textures.find(aipath.C_Str());
    if (loaded != loaded_textures.end())
    {
      textures.push_back(loaded->second);
    }
    else
    {
      tex->SetName(typeName);
      tex->SetPath(aipath.C_Str());
      tex->Load(aipath.C_Str(), _directory);
      textures.push_back(tex);
      loaded_textures[aipath.C_Str()] = tex;
    }
  }

  return textures;
}

}