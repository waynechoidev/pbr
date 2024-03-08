#pragma once

#include <iostream>

#include <GL/glew.h>

class Texture
{
public:
	Texture();

	void initialise(std::string fileLoc);
	void use();
	void clear();

	~Texture();

private:
	GLuint _textureID;
	int _width, _height, _bitDepth;
};