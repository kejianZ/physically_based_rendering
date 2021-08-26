#pragma once

#include <glm/glm.hpp>
using namespace glm;

struct Frame
{
    vec4 x;
    vec4 y;
    vec4 z;

    Frame(vec3 view, vec3 up)
    {
        z = vec4(normalize(-view), 0);
        x = vec4(normalize(cross(up, vec3(z))), 0);
        y = vec4(normalize(cross(vec3(z), vec3(x))), 0);
    }
};