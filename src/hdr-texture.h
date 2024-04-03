#pragma once

#include <iostream>
#include <GL/glew.h>

class HDRTexture
{
public:
	HDRTexture();

	void initialise(std::string name, std::string fileLoc);
	void use(GLuint programId, int index);
	void clear();

	~HDRTexture();

private:
	GLuint _textureID;
	int _width, _height, _bitDepth;
	std::string _name;
};