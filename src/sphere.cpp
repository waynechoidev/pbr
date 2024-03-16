#include "sphere.h"

Sphere::Sphere(GLfloat radius, GLuint numOfSlices, GLuint numOfStacks)
{
    const float dTheta = -2 * pi / float(numOfSlices);
    const float dPhi = -1 * pi / float(numOfStacks);

    for (GLuint j = 0; j <= numOfStacks; j++)
    {

        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), dPhi * float(j), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::vec3 stackStartPoint = glm::vec3(rotationMatrix * glm::vec4(0.0f, -radius, 0.0f, 1.0f));

        for (GLuint i = 0; i <= numOfSlices; i++)
        {
            Vertex v;

            glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), dTheta * float(i), glm::vec3(0.0f, 1.0f, 0.0f));
            v.position = glm::vec3(rotationMatrix * glm::vec4(stackStartPoint, 1.0f));

            v.normal = glm::normalize(v.position);
            v.tangent = calculateTangent(v.normal);
            v.texcoord = glm::vec2(1.0f - float(i) / numOfSlices, 1.0f - float(j) / numOfStacks);

            _vertices.push_back(v);
        }
    }

    for (GLuint j = 0; j < numOfStacks; j++)
    {

        const int offset = (numOfSlices + 1) * j;

        for (GLuint i = 0; i < numOfSlices; i++)
        {

            _indices.push_back(offset + i);
            _indices.push_back(offset + i + numOfSlices + 1);
            _indices.push_back(offset + i + 1 + numOfSlices + 1);

            _indices.push_back(offset + i);
            _indices.push_back(offset + i + 1 + numOfSlices + 1);
            _indices.push_back(offset + i + 1);
        }
    }
}

glm::vec3 Sphere::calculateTangent(const glm::vec3 &normal)
{
    glm::vec3 tangent = glm::vec3(1.0f, 0.0f, 0.0f);

    if (glm::abs(normal.x) < 0.999f || glm::abs(normal.y) < 0.999f)
    {
        tangent = glm::cross(normal, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    else
    {
        tangent = glm::cross(normal, glm::vec3(1.0f, 0.0f, 0.0f));
    }
    return glm::normalize(tangent);
}