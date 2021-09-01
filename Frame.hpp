#pragma once

#include <glm/glm.hpp>
using namespace glm;

struct Frame
{
    vec4 x; // direction
    vec4 y; // direction
    vec4 z; // direction

    Frame(vec4 view, vec4 up)
    {
        z = normalize(-view);
        vec3 x3 = normalize(cross(vec3(up), vec3(z)));
        x = vec4(x3, 0);
        vec3 y3 = normalize(cross(vec3(z), vec3(x)));
        y = vec4(y3, 0);
    }
    Frame() {}
};