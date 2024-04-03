#pragma once

#include <iostream>
#include <GL/glew.h>

class Texture
{
public:
	Texture();
	Texture(GLint channel);

	void initialise(std::string name, std::string fileLoc, bool isHDR);
	void use(GLuint programId, int index);
	void clear();

	~Texture();

private:
	GLuint _textureID, _textureChannel;
	int _width, _height, _bitDepth;
	std::string _name;
};