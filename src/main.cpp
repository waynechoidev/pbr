#define STB_IMAGE_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL

#include <filesystem>
#include "stb_image.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/quaternion.hpp>

#include "window.h"
#include "common.h"
#include "gui.h"
#include "program.h"
#include "sphere.h"
#include "texture.h"
#include "camera.h"
#include "cubemap.h"
#include "cube.h"
#include "hdr-texture.h"
#include "ray-picking.h"
#include "quad.h"

std::string hdrTextureSrc = "textures/air_museum_playground_4k.hdr";

std::string heightMapSrc = "textures/antique-grate1-height.png";
GLint heightMapChanel = GL_RGB;
std::string albedoMapSrc = "textures/antique-grate1-albedo.png";
GLint albedoMapChanel = GL_RGB;
std::string normalMapSrc = "textures/antique-grate1-normal-dx.png";
GLint normalMapChanel = GL_RGB;
std::string metallicMapSrc = "textures/antique-grate1-metallic.png";
GLint metallicMapChanel = GL_RGB;
std::string roughnessMapSrc = "textures/antique-grate1-roughness.png";
GLint roughnessMapChanel = GL_RGB;
std::string aoMapSrc = "textures/antique-grate1-ao.png";
GLint aoMapChanel = GL_RGB;

const GLfloat SPHERE_SCALE = 1.0;
const GLint WIDTH = 1920;
const GLint HEIGHT = 1080;

GLint envCubeSize = 2048;
GLint irradianceSize = 512;
GLint specularSize = 512;
GLint brdfSize = 512;

// Control
bool useDirectLight = true;
bool useEnvLight = true;
float heightScale = 0.03;
glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 2.0f);
bool dragStartFlag = false;
glm::vec3 prevMouseRayVector = glm::vec3(0.0f, 1.0f, 0.0f);

int main()
{
	Window mainWindow = Window(WIDTH, HEIGHT);
	mainWindow.initialise();

	Gui gui = Gui();
	gui.initialise(mainWindow.getWindow());

	Sphere sphere = Sphere(1.0f, 500, 500);
	sphere.initialise();

	Cube cube = Cube();
	Quad quad = Quad();

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

	Program irradianceProgram = Program();
	irradianceProgram.createFromFiles(currentDir / "shaders/cubemap.vert", currentDir / "shaders/irradiance.frag");
	GLuint irradianceProgramId = irradianceProgram.getId();
	GLuint irradianceViewLoc = glGetUniformLocation(irradianceProgramId, "view");
	GLuint irradianceProjectionLoc = glGetUniformLocation(irradianceProgramId, "projection");

	Program specularProgram = Program();
	specularProgram.createFromFiles(currentDir / "shaders/cubemap.vert", currentDir / "shaders/specular.frag");
	GLuint specularProgramId = specularProgram.getId();
	GLuint specularViewLoc = glGetUniformLocation(specularProgramId, "view");
	GLuint specularProjectionLoc = glGetUniformLocation(specularProgramId, "projection");
	GLuint specularRoughnessLoc = glGetUniformLocation(specularProgramId, "roughness");

	Program brdfProgram = Program();
	brdfProgram.createFromFiles(currentDir / "shaders/brdf.vert", currentDir / "shaders/brdf.frag");

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
	hdrTexture.initialise("hdrTexture", currentDir / hdrTextureSrc);

	// Env cube map
	Cubemap envCubemap = Cubemap();
	envCubemap.initialize(envCubeSize, "envCubemap");

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, envCubeSize, envCubeSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	equirectangularToCubemapProgram.use();
	glUniformMatrix4fv(equirectangularToCubemapProjectionLoc, 1, GL_FALSE, glm::value_ptr(captureProjection));
	hdrTexture.use(equirectangularToCubemapProgramId, 0);

	glViewport(0, 0, envCubeSize, envCubeSize); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glUniformMatrix4fv(equirectangularToCubemapViewLoc, 1, GL_FALSE, glm::value_ptr(captureViews[i]));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap.getId(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cube.draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Irradiance cubemap
	Cubemap irradianceCubemap = Cubemap();
	irradianceCubemap.initialize(irradianceSize, "irradianceCubemap");

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, irradianceSize, irradianceSize);

	irradianceProgram.use();
	glUniformMatrix4fv(irradianceProjectionLoc, 1, GL_FALSE, glm::value_ptr(captureProjection));
	envCubemap.use(irradianceProgramId, 0);

	glViewport(0, 0, irradianceSize, irradianceSize); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glUniformMatrix4fv(irradianceViewLoc, 1, GL_FALSE, glm::value_ptr(captureViews[i]));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceCubemap.getId(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cube.draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Specular cubemap
	Cubemap specularCubemap = Cubemap();
	specularCubemap.initialize(specularSize, "specularCubemap");

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, specularSize, specularSize);

	specularProgram.use();
	glUniformMatrix4fv(specularProjectionLoc, 1, GL_FALSE, glm::value_ptr(captureProjection));
	envCubemap.use(specularProgramId, 0);

	glViewport(0, 0, specularSize, specularSize); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = static_cast<unsigned int>(specularSize * std::pow(0.5, mip));
		unsigned int mipHeight = static_cast<unsigned int>(specularSize * std::pow(0.5, mip));
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		glUniform1f(specularRoughnessLoc, roughness);

		for (unsigned int i = 0; i < 6; ++i)
		{
			glUniformMatrix4fv(specularViewLoc, 1, GL_FALSE, glm::value_ptr(captureViews[i]));
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, specularCubemap.getId(), mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			cube.draw();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// BRDF
	HDRTexture brdfLUT = HDRTexture();
	brdfLUT.initialise("brdfLUT", brdfSize);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUT.getId(), 0);

	glViewport(0, 0, brdfSize, brdfSize);
	brdfProgram.use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	quad.draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, mainWindow.getBufferWidth(), mainWindow.getBufferHeight());

	Texture heightMap = Texture(heightMapChanel);
	heightMap.initialise("heightMap", currentDir / heightMapSrc, false);
	Texture albedoMap = Texture(albedoMapChanel);
	albedoMap.initialise("albedoMap", currentDir / albedoMapSrc, false);
	Texture normalMap = Texture(normalMapChanel);
	normalMap.initialise("normalMap", currentDir / normalMapSrc, false);
	Texture metallicMap = Texture(metallicMapChanel);
	metallicMap.initialise("metallicMap", currentDir / metallicMapSrc, false);
	Texture roughnessMap = Texture(roughnessMapChanel);
	roughnessMap.initialise("roughnessMap", currentDir / roughnessMapSrc, false);
	Texture aoMap = Texture(aoMapChanel);
	aoMap.initialise("aoMap", currentDir / aoMapSrc, false);

	mainProgram.use();
	heightMap.use(mainProgramId, 0);
	albedoMap.use(mainProgramId, 1);
	normalMap.use(mainProgramId, 2);
	metallicMap.use(mainProgramId, 3);
	roughnessMap.use(mainProgramId, 4);
	aoMap.use(mainProgramId, 5);
	irradianceCubemap.use(mainProgramId, 6);
	specularCubemap.use(mainProgramId, 7);
	brdfLUT.use(mainProgramId, 8);

	backgroundProgram.use();
	envCubemap.use(backgroundProgramId, 0);

	glUseProgram(0);

	// Model
	glm::vec3 translation = glm::vec3(0.0f);
	glm::vec3 scaling = glm::vec3(SPHERE_SCALE);
	glm::quat modelQuaternions = glm::angleAxis(0.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	// Projection
	float aspectRatio = (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

	// View
	Camera camera = Camera(glm::vec3(0.0f, 0.0f, 4.5f), glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3(0.0f, 0.0f, -1.0f));

	while (!mainWindow.getShouldClose())
	{
		glfwPollEvents();

		// Pick
		bool intersection = false;
		if (mainWindow.getMouseLeft())
		{
			glm::vec3 ray_wor = get_ray_from_mouse(mainWindow.getCursor(), projection, camera.calculateViewMatrix(), WIDTH, HEIGHT);
			glm::vec3 pickPoint = ray_sphere(camera.getPosition(), ray_wor, translation, SPHERE_SCALE, intersection);
			if (intersection)
			{
				if (dragStartFlag)
				{
					dragStartFlag = false;
					prevMouseRayVector = glm::normalize(pickPoint - translation);
				}
				else
				{
					glm::vec3 currentMouseRayVector = glm::normalize(pickPoint - translation);
					GLfloat theta = glm::acos(glm::dot(prevMouseRayVector, currentMouseRayVector));
					if (theta > 0.001f)
					{
						glm::vec3 axis = glm::normalize(glm::cross(prevMouseRayVector, currentMouseRayVector));
						modelQuaternions = glm::angleAxis(theta, axis) * modelQuaternions;
						prevMouseRayVector = glm::normalize(pickPoint - translation);
					}
				}
			}
		}
		else
		{
			dragStartFlag = true;
		}

		gui.update(heightScale, useDirectLight, useEnvLight, lightPos[0], &camera.getRotation()[1]);

		mainWindow.clear(0.0f, 0.0f, 0.0f, 1.0f);

		// Model
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(translation.x, translation.y, translation.z));
		model = glm::scale(model, scaling);
		model = model * glm::toMat4(modelQuaternions);

		mainProgram.use();
		mainProgram.bindVertexBuffers(model, projection, camera.calculateViewMatrix(), heightScale);
		mainProgram.bindFragmentBuffers(camera.getPosition(), lightPos, useDirectLight, useEnvLight);

		sphere.draw();

		// Background
		glDepthFunc(GL_LEQUAL);
		backgroundProgram.use();
		glUniformMatrix4fv(backgroundProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(backgroundViewLoc, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		cube.draw();
		glDepthFunc(GL_LESS);

		gui.render();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
