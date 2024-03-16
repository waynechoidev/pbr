#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

struct Material
{
	GLfloat ambient;
	GLfloat shininess;
	GLfloat diffuse;
	GLfloat specular;
};

struct Light
{
	glm::vec3 position;
	glm::vec3 direction;
	GLfloat strength;
	GLfloat fallOffStart;
	GLfloat fallOffEnd;
	GLfloat spotPower;
};

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec2 texcoord;
};
