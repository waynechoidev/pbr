#pragma once
#include <GL/glew.h>

class Cube
{
public:
    Cube();

    void draw();

    ~Cube() {}

private:
    unsigned int _cubeVAO;
    unsigned int _cubeVBO;
};
