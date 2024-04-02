#define STB_IMAGE_IMPLEMENTATION
#include <filesystem>
#include "stb_image.h"

#include "window.h"
#include "gui.h"
#include "program.h"
#include "sphere.h"
#include "texture.h"
#include "camera.h"

int main()
{
	Window mainWindow = Window(1920, 1080);
	mainWindow.initialise();

	Gui gui = Gui();
	gui.initialise(mainWindow.getWindow());

	Sphere sphere = Sphere(1.0f, 500, 500);
	sphere.initialise();

	std::filesystem::path currentDir = std::filesystem::path(__FILE__).parent_path();

	Program mainProgram = Program();
	mainProgram.createFromFiles(currentDir / "shaders/main.vert", currentDir / "shaders/main.frag");

	Texture heightMap = Texture(0, GL_RGB);
	heightMap.initialise(currentDir / "textures/antique-grate1-height.png");
	Texture albedoMap = Texture(1, GL_RGB);
	albedoMap.initialise(currentDir / "textures/antique-grate1-albedo.png");
	Texture normalMap = Texture(2, GL_RGB);
	normalMap.initialise(currentDir / "textures/antique-grate1-normal-dx.png");
	Texture metallicMap = Texture(3, GL_RGB);
	metallicMap.initialise(currentDir / "textures/antique-grate1-metallic.png");
	Texture roughnessMap = Texture(4, GL_RGB);
	roughnessMap.initialise(currentDir / "textures/antique-grate1-roughness.png");
	Texture aoMap = Texture(5, GL_RGB);
	aoMap.initialise(currentDir / "textures/antique-grate1-ao.png");

	// Model
	glm::vec3 translation = glm::vec3(0.0f);
	glm::vec3 scaling = glm::vec3(1.0f);
	glm::vec3 rotation = glm::vec3(0.0f);

	// Projection
	float aspectRatio = (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

	// View
	Camera camera = Camera(glm::vec3(0.0f, 0.0f, 4.5f), glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3(0.0f, 0.0f, -1.0f));

	// Control
	bool useDirectLight = true;
	bool useEnvLight = true;
	float heightScale = 0.03;
	glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 2.0f);

	while (!mainWindow.getShouldClose())
	{
		glfwPollEvents();

		gui.update(heightScale, useDirectLight, useEnvLight, lightPos[0]);

		mainWindow.clear(0.0f, 0.0f, 0.0f, 1.0f);

		// Model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(translation.x, translation.y, translation.z));
		model = glm::scale(model, scaling);
		model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

		mainProgram.use();
		mainProgram.bindVertexBuffers(model, projection, camera.calculateViewMatrix(), heightScale);
		mainProgram.bindFragmentBuffers(camera.getPosition(), lightPos, useDirectLight, useEnvLight);
		albedoMap.use();
		normalMap.use();
		heightMap.use();
		sphere.draw();
		gui.render();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}