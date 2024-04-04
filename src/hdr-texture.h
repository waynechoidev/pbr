#pragma once

#include <iostream>
#include <GL/glew.h>

class HDRTexture
{
public:
	HDRTexture();

	void initialise(std::string name, std::string fileLoc);
	void initialise(std::string name, GLint size);
	void use(GLuint programId, int index);
	GLuint getId() { return _textureID; }
	void clear();

	~HDRTexture();

private:
	GLuint _textureID;
	int _width, _height, _bitDepth;
	std::string _name;
};