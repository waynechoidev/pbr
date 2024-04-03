#define STB_IMAGE_IMPLEMENTATION
#include <filesystem>
#include "stb_image.h"
#include <glm/gtc/type_ptr.hpp>

#include "window.h"
#include "gui.h"
#include "program.h"
#include "sphere.h"
#include "texture.h"
#include "camera.h"
#include "cubemap.h"
#include "cube.h"
#include "hdr-texture.h"
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;

int main()
{
	Window mainWindow = Window(1920, 1080);
	mainWindow.initialise();

	Gui gui = Gui();
	gui.initialise(mainWindow.getWindow());

	Sphere sphere = Sphere(1.0f, 500, 500);
	sphere.initialise();

	Cube cube = Cube();

	std::filesystem::path currentDir = std::filesystem::path(__FILE__).parent_path();

	Program mainProgram = Program();
	mainProgram.createFromFiles(currentDir / "shaders/main.vert", currentDir / "shaders/main.frag");
	GLuint mainProgramId = mainProgram.getId();
	mainProgram.genVertexBuffers();
	mainProgram.genFragmentBuffers();

	Program equirectangularToCubemapProgram = Program();
	equirectangularToCubemapProgram.createFromFiles(currentDir / "shaders/cubemap.vert", currentDir / "shaders/equirectangular-to-cubemap.frag");
	GLuint equirectangularToCubemapProgramId = equirectangularToCubemapProgram.getId();
	GLuint equirectangularToCubemapViewLoc = glGetUniformLocation(equirectangularToCubemapProgramId, "view");
	GLuint equirectangularToCubemapProjectionLoc = glGetUniformLocation(equirectangularToCubemapProgramId, "projection");

	Program backgroundProgram = Program();
	backgroundProgram.createFromFiles(currentDir / "shaders/background.vert", currentDir / "shaders/background.frag");
	GLuint backgroundProgramId = backgroundProgram.getId();
	GLuint backgroundProjectionLoc = glGetUniformLocation(backgroundProgramId, "projection");
	GLuint backgroundViewLoc = glGetUniformLocation(backgroundProgramId, "view");

	// setup framebuffer
	unsigned int captureFBO;
	unsigned int captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	int bufferSize = 4096;
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, bufferSize, bufferSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	// set up projection and view matrices for capturing data onto the 6 cubemap face directions
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

	HDRTexture hdrTexture = HDRTexture();
	hdrTexture.initialise("hdrTexture", currentDir / "textures/forgotten_miniland_8k.hdr");

	Cubemap envCubemap = Cubemap();
	envCubemap.initialize(bufferSize);

	equirectangularToCubemapProgram.use();
	glUniformMatrix4fv(equirectangularToCubemapProjectionLoc, 1, GL_FALSE, glm::value_ptr(captureProjection));
	hdrTexture.use(equirectangularToCubemapProgramId, 0);

	glViewport(0, 0, bufferSize, bufferSize); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glUniformMatrix4fv(equirectangularToCubemapViewLoc, 1, GL_FALSE, glm::value_ptr(captureViews[i]));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap.getId(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cube.draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Texture heightMap = Texture(GL_RGB);
	heightMap.initialise("heightMap", currentDir / "textures/antique-grate1-height.png", false);
	Texture albedoMap = Texture(GL_RGB);
	albedoMap.initialise("albedoMap", currentDir / "textures/antique-grate1-albedo.png", false);
	Texture normalMap = Texture(GL_RGB);
	normalMap.initialise("normalMap", currentDir / "textures/antique-grate1-normal-dx.png", false);
	Texture metallicMap = Texture(GL_RGB);
	metallicMap.initialise("metallicMap", currentDir / "textures/antique-grate1-metallic.png", false);
	Texture roughnessMap = Texture(GL_RGB);
	roughnessMap.initialise("roughnessMap", currentDir / "textures/antique-grate1-roughness.png", false);
	Texture aoMap = Texture(GL_RGB);
	aoMap.initialise("aoMap", currentDir / "textures/antique-grate1-ao.png", false);

	mainProgram.use();
	heightMap.use(mainProgramId, 0);
	albedoMap.use(mainProgramId, 1);
	normalMap.use(mainProgramId, 2);
	metallicMap.use(mainProgramId, 3);
	roughnessMap.use(mainProgramId, 4);
	aoMap.use(mainProgramId, 5);

	// Model
	glm::vec3 translation = glm::vec3(0.0f);
	glm::vec3 scaling = glm::vec3(1.0f);
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);

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

	glViewport(0, 0, mainWindow.getBufferWidth(), mainWindow.getBufferHeight());

	while (!mainWindow.getShouldClose())
	{
		gui.update(heightScale, useDirectLight, useEnvLight, lightPos[0], camera.getRotation());

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

		sphere.draw();

		// Background
		glDepthFunc(GL_LEQUAL);
		backgroundProgram.use();
		envCubemap.use(backgroundProgramId, 0);
		glUniformMatrix4fv(backgroundProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(backgroundViewLoc, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		cube.draw();
		glDepthFunc(GL_LESS);

		gui.render();

		glUseProgram(0);

		mainWindow.swapBuffers();
		glfwPollEvents();
	}

	return 0;
}