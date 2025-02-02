﻿
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Mesh* cubeBlue;
	Mesh* cubeGreen;
	Mesh* cubeRed;
	Mesh* cubeYellow;

	Example()
	{
		title = "uniform buffer object";
		settings.vsync = false;
		defaultClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/18.uniform_buffer_object/";
	}
	~Example()
	{
		delete(cubeBlue);
		delete(cubeGreen);
		delete(cubeRed);
		delete(cubeYellow);
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// setup camera
		camera->movementSpeed = 2.0f;
		camera->rotationSpeed = 1.0f;
		camera->setPosition(glm::vec3(0.0f, 0.0f, 3.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		std::vector<GLfloat> vertexAttrs = {
			// positions         
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f
		};

		std::vector<Vertex> vertices = {};
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexAttrs.size() / 3); i++)
		{
			Vertex vertex;
			vertex.Position = glm::vec3(vertexAttrs[i * 3], vertexAttrs[i * 3 + 1], vertexAttrs[i * 3 + 2]);
			vertices.push_back(vertex);
		}

		std::unordered_map<Material::ShaderType, std::string> blueShaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "ubo.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "blue.frag" }
		};

		std::unordered_map<Material::ShaderType, std::string> greenShaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "ubo.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "green.frag" }
		};

		std::unordered_map<Material::ShaderType, std::string> redShaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "ubo.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "red.frag" }
		};

		std::unordered_map<Material::ShaderType, std::string> yellowShaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "ubo.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "yellow.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> texturePaths =
		{
			
		};

		// create cubeBlue material
		std::shared_ptr<Material> blueMaterial = std::make_shared<Material>(blueShaderPaths, texturePaths);

		// create cubeGreen material
		std::shared_ptr<Material> greenMaterial = std::make_shared<Material>(greenShaderPaths, texturePaths);

		// create cubeRed material
		std::shared_ptr<Material> redMaterial = std::make_shared<Material>(redShaderPaths, texturePaths);

		// create cubeYellow material
		std::shared_ptr<Material> yellowMaterial = std::make_shared<Material>(yellowShaderPaths, texturePaths);
		
		// because all the uniform block index in shaders are same, so we just need to get it once
		GLuint uniformBlockIndexBlue = glGetUniformBlockIndex(blueMaterial->getProgramID(), "mixColor");

		// create the MixColor ubo, use default shared memory layout
		GLuint uboMixColor;
		glGenBuffers(1, &uboMixColor);
		glBindBuffer(GL_UNIFORM_BUFFER, uboMixColor);
		GLint blockSize;
		// get the needed size of uniform block
		glGetActiveUniformBlockiv(blueMaterial->getProgramID(), uniformBlockIndexBlue, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
		// pre-allocate memory
		glBufferData(GL_UNIFORM_BUFFER, blockSize, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		// set binding point to 0
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMixColor);

		const GLchar* const names[] = {
			"additionalColor",
			"mixValue"
		};
		GLuint indices[2];
		glGetUniformIndices(blueMaterial->getProgramID(), 2, names, indices);
		GLint offsets[2];
		glGetActiveUniformsiv(blueMaterial->getProgramID(), 2, indices, GL_UNIFORM_OFFSET, offsets);
		glm::vec4 additionalColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		GLfloat mixValue = 0.5f;
		glBindBuffer(GL_UNIFORM_BUFFER, uboMixColor);
		// fill the data
		glBufferSubData(GL_UNIFORM_BUFFER, offsets[0], sizeof(glm::vec4), &additionalColor[0]);
		glBufferSubData(GL_UNIFORM_BUFFER, offsets[1], sizeof(GLfloat), &mixValue);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		
		// create cubeBlue mesh
		cubeBlue = Mesh::createWithData(vertices, {}, Mesh::DrawType::Arrays, blueMaterial);
		cubeBlue->setPosition(glm::vec3(-0.75f, 0.75f, 0.0f));

		// create cubeGreen mesh
		cubeGreen = Mesh::createWithData(vertices, {}, Mesh::DrawType::Arrays, greenMaterial);
		cubeGreen->setPosition(glm::vec3(0.75f, 0.75f, 0.0f));

		// create cubeRed mesh
		cubeRed = Mesh::createWithData(vertices, {}, Mesh::DrawType::Arrays, redMaterial);
		cubeRed->setPosition(glm::vec3(-0.75f, -0.75f, 0.0f));

		// create cubeYellow mesh
		cubeYellow = Mesh::createWithData(vertices, {}, Mesh::DrawType::Arrays, yellowMaterial);
		cubeYellow->setPosition(glm::vec3(0.75f, -0.75f, 0.0f));
	}

	virtual void render(float deltaTime) override
	{
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		cubeBlue->render(deltaTime);
		cubeGreen->render(deltaTime);
		cubeRed->render(deltaTime);
		cubeYellow->render(deltaTime);
	}
};

Example* example;
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	example = new Example();
	example->setupValidation();
	if (!example->setupGLFW() ||
		!example->loadGLESFunctions() ||
		!example->setupImGui())
	{
		return 0;
	}
	example->prepare();
	example->renderLoop();
	delete(example);
	return 0;
}