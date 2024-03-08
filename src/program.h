#pragma once

#include "common.h"
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Program
{
public:
	Program();

	void createFromString(std::string vertexCode, std::string fragmentCode);
	void createFromFiles(std::string vertexLocation, std::string fragmentLocation);

	std::string readFile(std::string fileLocation);

	void genVertexBuffers();
	void genFragmentBuffers();
	void use();
	void bindVertexBuffers(glm::mat4 model, glm::mat4 projection, glm::mat4 view);
	void bindFragmentBuffers(bool useTexture, glm::vec3 viewPosition, const Material &material, const Light &light);

	~Program();

private:
	GLuint _programID;
	GLuint _uboMatrices, _uboFragment, _uboMaterial, _uboLight;

	void compileShader(std::string vertexCode, std::string fragmentCode);
	void addShader(GLuint theProgram, std::string shaderCode, GLenum shaderType);
	void clear();
};
