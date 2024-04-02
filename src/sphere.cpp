#include "sphere.h"

Sphere::Sphere(GLfloat radius, GLuint numOfSlices, GLuint numOfStacks)
{
    const float dTheta = 2 * pi / float(numOfSlices);
    const float dPhi = pi / float(numOfStacks);

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
            v.texcoord = glm::vec2(1.0f - float(i) / numOfSlices, 1.0f - float(j) / numOfStacks);

            _vertices.push_back(v);
        }
    }

    for (unsigned int j = 0; j < numOfStacks; ++j)
    {
        const unsigned int offset = j * (numOfSlices + 1);
        for (unsigned int i = 0; i < numOfSlices; ++i)
        {
            const unsigned int index1 = offset + i;
            const unsigned int index2 = offset + i + 1;
            const unsigned int index3 = offset + (i + 1) % (numOfSlices + 1) + (numOfSlices + 1);
            const unsigned int index4 = index3 - 1;

            _indices.push_back(index1);
            _indices.push_back(index3);
            _indices.push_back(index2);

            _indices.push_back(index1);
            _indices.push_back(index4);
            _indices.push_back(index3);

            const Vertex &v1 = _vertices[index1];
            const Vertex &v2 = _vertices[index2];
            const Vertex &v3 = _vertices[index3];
            const Vertex &v4 = _vertices[index4];

            _vertices[index1].tangent = calculateTangent(v1.position, v2.position, v3.position, v1.texcoord, v2.texcoord, v3.texcoord);
            _vertices[index2].tangent = _vertices[index1].tangent;
            _vertices[index3].tangent = _vertices[index1].tangent;

            _vertices[index1].tangent = calculateTangent(v1.position, v3.position, v4.position, v1.texcoord, v3.texcoord, v4.texcoord);
            _vertices[index3].tangent = _vertices[index1].tangent;
            _vertices[index4].tangent = _vertices[index1].tangent;
        }
    }
}

glm::vec3 Sphere::calculateTangent(const glm::vec3 &pos1, const glm::vec3 &pos2, const glm::vec3 &pos3,
                                   const glm::vec2 &uv1, const glm::vec2 &uv2, const glm::vec2 &uv3)
{
    const glm::vec3 edge1 = pos2 - pos1;
    const glm::vec3 edge2 = pos3 - pos1;
    const glm::vec2 deltaUV1 = uv2 - uv1;
    const glm::vec2 deltaUV2 = uv3 - uv1;

    const float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    glm::vec3 tangent;
    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    const glm::vec3 normal = glm::cross(edge1, edge2);
    tangent = glm::normalize(tangent - normal * glm::dot(normal, tangent));

    return tangent;
}