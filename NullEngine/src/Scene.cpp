#include "Scene.h"

#include <iostream>
#include <random>

void Scene::SetUp()
{
	// obtain resources path
	std::string root = R"(../Resources/)";

	// texture loading from image
	NullEngine::Texture texture1("container", root + "container.jpg");
	NullEngine::Texture texture2("AwesomeFace", root + "awesomeface.png", true);
	texture1.Load();
	texture2.Load();

	containerDiffuseMap = NullEngine::Texture("containerWood", root + "container2.png");
	containerSpecularMap = NullEngine::Texture("containerSteelBorder", root + "container2_specular.png");
	containerEmissionMap = NullEngine::Texture("containerEmission", root + "matrix_container.png");
	containerDiffuseMap.Load();
	containerSpecularMap.Load();
	containerEmissionMap.Load();

	skyBoxCubeMap = NullEngine::CubeMap("LearnOpenGLskyBox", "skybox", root);
	skyBoxCubeMap.Load();

	skyBoxCubeMap2 = NullEngine::CubeMap("LearnOpenGLskyBox2", "skybox2", root, ".png");
	skyBoxCubeMap2.Load();

	guitarBag = NullEngine::Model("../Resources/backpack/backpack.obj", nullptr, true);
	singapore = "../Resources/singapore/untitled.obj";
	// Model destructor("../Resources/destructor-pesado-imperial-isd-1/Destructor imperial ISD 1.obj");
	// Model sponza("../Resources/sponza/source/sponza.fbx", "../Resources/sponza/textures", false);

	std::string shaderRoot = "../LearnOpenGL_guide/shaders/";
	shaderSingleColor = NullEngine::Shader((shaderRoot + "2.stencil_testing.vs").c_str(), (shaderRoot + "2.stencil_single_color.fs").c_str());

	skyBox = {_vertices[9]};

	glGenVertexArrays(2, VAOs);

	glGenBuffers(2, VBOs);

	glGenBuffers(2, EBO);

	// 1. bind Vertex Array Object
	glBindVertexArray(VAOs[0]);
	// 2. copy our vertices array in a buffer for OpenGL to use

	auto &cubeLsource = _vertices[4];
	auto &cubeOb = _vertices[6];

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, cubeOb.size() * sizeof(float), cubeOb.data(), GL_STATIC_DRAW);

	// 3. then set our vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	/*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);*/

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glGenVertexArrays(1, &screenQuadVAO);
	glGenBuffers(1, &screenQuadVBO);
	glBindVertexArray(screenQuadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);

	auto &quadVertices = _vertices[7];
	glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(float), quadVertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

	glGenFramebuffers(1, &framebuf);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuf);

	const GLsizei Width = _width, Height = _height;
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
	unsigned mirrorQuadVBO;
	glGenVertexArrays(1, &mirrorQuadVAO);
	glGenBuffers(1, &mirrorQuadVBO);
	glBindVertexArray(mirrorQuadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mirrorQuadVBO);

	auto &mirrorVertices = _vertices[8];
	glBufferData(GL_ARRAY_BUFFER, mirrorVertices.size() * sizeof(float), mirrorVertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

	glGenFramebuffers(1, &mirrorBuf);
	glBindFramebuffer(GL_FRAMEBUFFER, mirrorBuf);

	mirrorWidth = _width;
	mirrorHeight = _height;
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
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	_shaders[(int)NullEngine::ShadersTypes::VertexFragment0]->Use();
	_shaders[(int)NullEngine::ShadersTypes::VertexFragment0]->SetInt("texture1", 0);
	_shaders[(int)NullEngine::ShadersTypes::VertexFragment0]->SetInt("texture2", 1);

	objectShader = _shaders[(int)NullEngine::ShadersTypes::LightingCube].get();
	lightSourceCube = _shaders[(int)NullEngine::ShadersTypes::LightSource].get();
	skyBoxShader = _shaders[(int)NullEngine::ShadersTypes::SkyBoxS].get();
	cmReflectRefract = _shaders[(int)NullEngine::ShadersTypes::CubeMapReflect].get();

	std::vector<NullEngine::Shader *> activeShaders = {objectShader, lightSourceCube}; // _shaders[0].get()};

	// this enables Z-buffer so that faces overlap correctly when projected to the screen
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	time = 0.0f;
	float timeLast = 0.0f;
	deltap = 0.0f;

	// randomize some cube positions
	std::random_device r;
	std::mt19937 gen(r());
	float range = 5.0f;
	std::uniform_real_distribution<float> uniform_dist(-range, range);

	for (int i = 0; i < _materials.size(); ++i)
	{
		glm::vec3 randvec((float)uniform_dist(gen), (float)uniform_dist(gen), (float)uniform_dist(gen));
		randvecs.push_back(randvec);
	}

	std::uniform_int_distribution<int> uni_sgn(1, 2);
	std::uniform_real_distribution<float> uni_rad(5.0f, 20.0f);
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

	glGenBuffers(1, &uboVP);
	glBindBuffer(GL_UNIFORM_BUFFER, uboVP);
	// allocate memory for two float4x4 matrices
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboVP, 0, 2 * sizeof(glm::mat4));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}