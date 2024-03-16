#pragma once

#include "mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Sphere : public Mesh
{
public:
	Sphere(GLfloat radius, GLuint numOfSlices, GLuint numOfStacks);
	Sphere(GLfloat radius) : Sphere(radius, 30, 30) {}
	Sphere() : Sphere(1.0f) {}

	~Sphere() {}

private:
	glm::vec3 calculateTangent(const glm::vec3 &normal, const glm::vec3 &position);

	GLfloat pi = 3.14159265358979323846f;
};
