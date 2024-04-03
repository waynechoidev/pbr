#pragma once

#include <iostream>
#include <GL/glew.h>

class Cubemap
{
public:
    Cubemap(){};
    void initialize(int size);

    GLuint getId() { return _textureID; }
    void use(GLuint programId, int index);

    ~Cubemap() {}

private:
    GLuint _textureID;
    int _width, _height, _bitDepth;
    std::string _name;
};
