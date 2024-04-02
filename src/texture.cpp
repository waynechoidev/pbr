#include "texture.h"
#include "stb_image.h"

Texture::Texture()
{
	Texture(0, GL_RGB);
}

Texture::Texture(int index, GLint channel)
{
	_textureID = 0;
	_textureIndex = GL_TEXTURE0 + index;
	_textureChannel = channel;
	_width = 0;
	_height = 0;
	_bitDepth = 0;
}

void Texture::initialise(std::string fileLoc)
{
	unsigned char *texData = stbi_load(fileLoc.c_str(), &_width, &_height, &_bitDepth, 0);
	if (!texData)
	{
		std::cout << "Failed to find : " << fileLoc << std::endl;
		return;
	}

	glGenTextures(1, &_textureID);
	glBindTexture(GL_TEXTURE_2D, _textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, _textureChannel, _width, _height, 0, _textureChannel, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(texData);
}

void Texture::use()
{
	glActiveTexture(_textureIndex);
	glBindTexture(GL_TEXTURE_2D, _textureID);
}

void Texture::clear()
{
	glDeleteTextures(1, &_textureID);
	_textureID = 0;
	_width = 0;
	_height = 0;
	_bitDepth = 0;
}

Texture::~Texture()
{
	clear();
}