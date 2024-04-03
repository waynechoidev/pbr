#pragma once

#include <glm/glm.hpp>

// https://github.com/capnramses/antons_opengl_tutorials_book/blob/master/07_ray_picking/main.cpp
glm::vec3 get_ray_from_mouse(glm::vec2 cursor, glm::mat4 projection, glm::mat4 view, GLint width, GLint height)
{
    // screen space (viewport coordinates)
    GLfloat x = cursor.x * 2.0f / width - 1.0f;
    GLfloat y = 1.0f - cursor.y * 2.0f / height;
    GLfloat z = 1.0f;
    // normalised device space
    glm::vec3 ray_nds = glm::vec3(x, y, z);
    // clip space
    glm::vec4 ray_clip = glm::vec4(ray_nds[0], ray_nds[1], -1.0, 1.0);
    // eye space
    glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
    ray_eye = glm::vec4(ray_eye[0], ray_eye[1], -1.0, 0.0);
    // world space
    glm::vec3 ray_wor = glm::vec3(inverse(view) * ray_eye);
    ray_wor = glm::normalize(ray_wor);
    return ray_wor;
}

// https://github.com/capnramses/antons_opengl_tutorials_book/blob/master/07_ray_picking/main.cpp
glm::vec3 ray_sphere(glm::vec3 ray_origin_wor, glm::vec3 ray_direction_wor, glm::vec3 sphere_centre_wor, GLfloat sphere_radius, bool &intersection)
{
    // work out components of quadratic
    glm::vec3 dist_to_sphere = ray_origin_wor - sphere_centre_wor;
    GLfloat b = glm::dot(ray_direction_wor, dist_to_sphere);
    GLfloat c = glm::dot(dist_to_sphere, dist_to_sphere) - sphere_radius * sphere_radius;
    GLfloat b_squared_minus_c = b * b - c;

    // check for "imaginary" answer. == ray completely misses sphere
    if (b_squared_minus_c < 0.0f)
    {
        intersection = false;
        return glm::vec3(0.0f);
    }

    // check for ray hitting twice (in and out of the sphere)
    if (b_squared_minus_c > 0.0f)
    {
        // get the 2 intersection distances along ray
        GLfloat t_a = -b + sqrt(b_squared_minus_c);
        GLfloat t_b = -b - sqrt(b_squared_minus_c);

        // if behind viewer, throw one or both away
        if (t_a < 0.0)
        {
            if (t_b < 0.0)
            {
                intersection = false;
                return glm::vec3(0.0f);
            }
        }
        else if (t_b < 0.0)
        {
            return ray_origin_wor + ray_direction_wor * t_a;
        }
        else
        {
            intersection = true;
            return ray_origin_wor + ray_direction_wor * t_b;
        }
    }

    // check for ray hitting once (skimming the surface)
    if (b_squared_minus_c == 0.0f)
    {
        // if behind viewer, throw away
        GLfloat t = -b + sqrt(b_squared_minus_c);
        if (t < 0.0f)
        {
            intersection = false;
            return glm::vec3(0.0f);
        }
        intersection = true;
        return ray_origin_wor + ray_direction_wor * t;
    }

    // note: could also check if ray origin is inside sphere radius
    intersection = false;
    return glm::vec3(0.0f);
}
