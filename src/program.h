#pragma once

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

	void createFromFiles(std::string vertexLocation, std::string fragmentLocation);

	std::string readFile(std::string fileLocation);

	void genVertexBuffers();
	void genFragmentBuffers();
	void bindVertexBuffers(glm::mat4 model, glm::mat4 projection, glm::mat4 view, float &heightScale);
	void bindFragmentBuffers(glm::vec3 campos, glm::vec3 lightPos, bool useDirectLight, bool useEnvLight);
	GLuint getId() { return _programID; }

	void use();

	~Program();

private:
	GLuint _programID;
	GLuint _uboMatrices, _uboFragment, _uboMaterial, _uboLight;

	void compileShader(std::string vertexCode, std::string fragmentCode);
	void addShader(GLuint theProgram, std::string shaderCode, GLenum shaderType);
	void clear();
};
