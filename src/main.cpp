#define STB_IMAGE_IMPLEMENTATION
#include <filesystem>
#include "stb_image.h"

#include "window.h"
#include "gui.h"
#include "program.h"
#include "sphere.h"
#include "texture.h"
#include "camera.h"

bool useTexture = false;

int main()
{
	Window mainWindow = Window(1366, 768);
	mainWindow.initialise();

	Gui gui = Gui();
	gui.initialise(mainWindow.getWindow());

	Sphere sphere = Sphere(1.0f);
	sphere.initialise();

	std::filesystem::path currentDir = std::filesystem::path(__FILE__).parent_path();

	Program mainProgram = Program();
	mainProgram.createFromFiles(currentDir / "shaders/vert.glsl", currentDir / "shaders/frag.glsl");
	mainProgram.genVertexBuffers();

	Texture earthTexture = Texture();
	earthTexture.initialise(currentDir / "textures/map.jpg");

	// Model
	glm::vec3 translation = {0.0f, 0.0f, 0.0f};
	glm::vec3 scaling = {0.4f, 0.4f, 0.4f};
	glm::vec3 rotation = {0.0f, 0.0f, 0.0f};

	// Projection
	float aspectRatio = (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

	Camera camera = Camera(glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3(0.0f, 0.0f, -1.0f));

	while (!mainWindow.getShouldClose())
	{
		glfwPollEvents();

		gui.update(useTexture);

		mainWindow.clear(0.0f, 0.0f, 0.0f, 1.0f);

		// Model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(translation.x, translation.y, translation.z));
		model = glm::scale(model, scaling);
		model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

		mainProgram.use();
		mainProgram.bindVertexBuffers(model, projection, camera.calculateViewMatrix());
		// mainProgram.bindFragmentBuffers(useTexture, camera.getPosition(), material);
		earthTexture.use();
		sphere.draw();
		gui.render();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}