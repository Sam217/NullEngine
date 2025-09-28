#pragma once
#include "Model.h"
#include "Shader.h"
#include "SkyBox.h"

#include <vector>
#include <glm/vec3.hpp>

class Scene
{
public:
	void SetUp();

	//! Params
	NullEngine::Texture containerDiffuseMap;
	NullEngine::Texture containerSpecularMap;
	NullEngine::Texture containerEmissionMap;
	NullEngine::CubeMap skyBoxCubeMap;
	NullEngine::CubeMap skyBoxCubeMap2;
	NullEngine::Model guitarBag;
	NullEngine::Model singapore;
	NullEngine::Shader shaderSingleColor;
	NullEngine::SkyBox skyBox;
	unsigned int VAOs[2];
	unsigned int VBOs[2];
	unsigned int EBO[2];
	unsigned screenQuadVAO;
	unsigned screenQuadVBO;
	unsigned framebuf;
	unsigned textureColor;
	unsigned mirrorQuadVAO;
	unsigned mirrorBuf;
	GLsizei mirrorWidth;
	GLsizei mirrorHeight;
	unsigned texMirror;
	NullEngine::Shader *objectShader;
	NullEngine::Shader *lightSourceCube;
	NullEngine::Shader *skyBoxShader;
	NullEngine::Shader *cmReflectRefract;
	float time;
	float deltap;
	std::vector<glm::vec3> randvecs;
	int randsgn[4];
	int randRadius[4];
	unsigned uboVP;
};
