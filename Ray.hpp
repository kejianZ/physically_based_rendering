#pragma once
#include <glm/glm.hpp>
using namespace glm;

class Ray
{
public:
    vec3 Origin;
    vec3 Direction;

    Ray(vec3 origin, vec3 direction, bool dir = true);
    Ray();
    vec3 pos_at(float t);
};
