#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "IEngine.h"
#include "Shader.h"
#include "Camera.h"
#include "Scene.h"

struct ImGuiIO;

// Basic Phong-shading material
struct Material
{
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;
};

namespace NullEngine {
	class Model;


	class Engine : IEngine
	{
		friend class Camera;

	public:
		//! Scene positions?
		struct Positions
		{
			std::vector<glm::vec3> cubePositions;
			std::vector<glm::vec3> pointLightPositions;
			glm::vec3 lightPos;
		};
		//! Scene input
		struct SceneParams
		{
			//! Textures scene input
			std::unique_ptr<Texture> containerDiffuseMap;
			std::unique_ptr<Texture> containerSpecularMap;
			std::unique_ptr<Texture> containerEmissionMap;
			std::unique_ptr<CubeMap> skyBoxCubeMap;
			std::unique_ptr<CubeMap> skyBoxCubeMap2;
			//! Models scene input
			std::unique_ptr<Model> guitarBag;
			std::unique_ptr<Model> singapore;
			std::unique_ptr<Shader> shaderSingleColor;
			Shader* lightSourceCube;
			Shader* objectShader;
			Shader* skyBoxShader;
			Shader* cmReflectRefract;

			std::unique_ptr<SkyBox> skyBox;
			unsigned int VAOs[2]; // sould be 2
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
			unsigned uboVP;
		};
		//! Additional params...
		struct AdditionalParams
		{
			float time;
			float deltap;
			std::vector<glm::vec3> randvecs;
			int randsgn[4];
			int randRadius[4];
		};

		static Engine* _engineContext;

		//! Ctor
		Engine() { _camera = Camera(_width / 2.0f, _height / 2.0f); };

		//! Dtor
		virtual ~Engine() override = default;
		//! Later initialization
		virtual void Init() override;
		//! Main
		virtual int Main() override;
		//! GetGlfwWindow
		GLFWwindow* GetGlfwWindow() { return _window; }

	private:
		//! Init GL Framework
		void InitGLFW();
		//! Create shaders
		void CreateShaders();
		void InitPhongMaterials();
		void InitPositions();
		//!
		void InitVertices();
		void InitImGui();

		//! Set up some scene #TODO replace by Scene class
		void SetUpScene(SceneParams& sceneParams, AdditionalParams& additionals);
		//! Process input
		void processInput(float dt);
		void ShowAppDockSpace(bool* p_open);

		static void Framebuffer_size_callback(GLFWwindow* window, int width, int height);

		static void Mouse_callback(GLFWwindow* window, double xpos, double ypos);
		static void Scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	private:
		//! GUI
		ImGuiIO* _io = nullptr;
		bool _captureMouse = true;
		//! Window
		GLFWwindow* _window = nullptr;
		//! Camera
		Camera _camera;

		//! ligts modification params
		int _lightColorIntensity = 100;
		int _lightAmbIntensity = 100;
		int _lightDiffIntensity = 100;
		int _lightSpecIntensity = 100;

		int _spotLightColorIntensity = 100;

		bool _pause = false;
		//
		int _width = 1920;
		//
		int _height = 1080;

		//! list of shaders & effects
		std::vector<std::unique_ptr<Shader>> _shaders;
		const Shader* _currentEffect = nullptr;
		//!
		std::vector<std::vector<float>> _vertices;
		std::vector<Material> _materials;
		Positions _positions;

		//! Some Scene #TODO
		Scene someScene;
	};

	enum class ShadersTypes
	{
		VertexFragment0,
		VertexFragment1,
		LightingCube,
		LightSource,
		LightingCubeExplosion,
		LightingCubeGouraud,

		SimpleShader,
		EffectNegative,
		EffectGreyScale,
		EffectGreyScaleWeighted,
		EffectSharpen,
		EffectBlur,
		EffectEdge,

		SkyBoxS,
		CubeMapReflect,
		CubeMapRefract,
		VisualizeNormals,

		NShaderTypes
	};

} // namespace NullEngine
