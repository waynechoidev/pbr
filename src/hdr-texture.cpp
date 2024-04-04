#include "hdr-texture.h"
#include "stb_image.h"

HDRTexture::HDRTexture()
{
	_textureID = 0;
	_width = 0;
	_height = 0;
	_bitDepth = 0;
}

void HDRTexture::initialise(std::string name, std::string fileLoc)
{
	_name = name;

	float *texData = stbi_loadf(fileLoc.c_str(), &_width, &_height, &_bitDepth, 0);
	if (!texData)
	{
		std::cout << "Failed to find : " << fileLoc << std::endl;
		return;
	}

	glGenTextures(1, &_textureID);
	glBindTexture(GL_TEXTURE_2D, _textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _width, _height, 0, GL_RGB, GL_FLOAT, texData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(texData);
}

void HDRTexture::initialise(std::string name, GLint size)
{
	_name = name;

	glGenTextures(1, &_textureID);
	glBindTexture(GL_TEXTURE_2D, _textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, size, size, 0, GL_RG, GL_FLOAT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void HDRTexture::use(GLuint programId, int index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, _textureID);
	auto uniformLoc = glGetUniformLocation(programId, _name.c_str());
	glUniform1i(uniformLoc, index);
}

void HDRTexture::clear()
{
	glDeleteTextures(1, &_textureID);
	_textureID = 0;
	_width = 0;
	_height = 0;
	_bitDepth = 0;
}

HDRTexture::~HDRTexture()
{
	clear();
}