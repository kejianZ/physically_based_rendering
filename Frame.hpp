#pragma once

#include <glm/glm.hpp>
using namespace glm;

struct Frame
{
    vec3 x; // direction
    vec3 y; // direction
    vec3 z; // direction

    Frame(vec3 view, vec3 up)
    {
        z = normalize(-view);
        x = normalize(cross(up, vec3(z)));
        y = normalize(cross(vec3(z), vec3(x)));
    }
};