#pragma once

#include <iostream>

#include <GL/glew.h>

class Texture
{
public:
	Texture();
	Texture(int index, GLint channel);

	void initialise(std::string fileLoc);
	void use();
	void clear();

	~Texture();

private:
	GLuint _textureID, _textureIndex, _textureChannel;
	int _width, _height, _bitDepth;
};