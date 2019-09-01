﻿
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	Model* planeModel;

	Example()
	{
		title = "normal mapping";
		settings.vsync = false;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/16.normal_mapping/";
		texturesDirectory = getResourcesPath(ResourceType::Texture) + "/16.normal_mapping/";
	}
	~Example()
	{
		delete(planeModel);
	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// setup camera
		camera->movementSpeed = 2.0f;
		camera->rotationSpeed = 0.5f;
		camera->setPosition(glm::vec3(0.0f, 2.0f, 2.0f));
		camera->setRotation(glm::vec3(-60.0f, -90.0f, 0.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		std::unordered_map<Material::ShaderType, std::string> shaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "plane.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "plane.frag" }
		};

		planeModel = Model::createWithFile(modelsDirectory + "/rocks_plane/rocks_plane.obj", shaderPaths);
		planeModel->setRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
		planeModel->setScale(glm::vec3(0.2f, 0.2f, 0.2f));

		Texture* diffuseMap = Texture::createWithFile(modelsDirectory + "/rocks_plane/rocks_color_bc3_unorm.png", Texture::Type::Diffuse);
		Texture* normalMap = Texture::createWithFile(modelsDirectory + "/rocks_plane/rocks_normal_height_rgba.png", Texture::Type::Normal);

		planeModel->setInteger("diffuseMap_0", 0);
		planeModel->setInteger("normalMap_0", 1);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap->getID());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normalMap->getID());
	}

	virtual void render(float deltaTime) override
	{
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		planeModel->setVec3("lightPos", glm::vec3(sin(glm::radians(timePassed * 360.0f)) * 1.5f, 10.0f, cos(glm::radians(timePassed * 360.0f)) * 1.5f));
		planeModel->setVec3("viewPos", camera->getPosition());
		planeModel->draw(deltaTime, true);
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