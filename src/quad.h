#pragma once
#include <GL/glew.h>

class Quad
{
public:
    Quad();

    void draw();

    ~Quad() {}

private:
    unsigned int _quadVAO;
    unsigned int _quadVBO;
};
