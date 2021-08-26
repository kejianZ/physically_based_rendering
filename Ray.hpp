#pragma once

#include <glm/glm.hpp>
using namespace glm;

class Ray
{
private:
    vec4 Origin;
    vec4 Direction;
public:
    Ray(vec4 origin, vec4 direction);
    Ray() {}
};

Ray::Ray(vec4 origin, vec4 direction)
{
    Origin = origin;
    if (direction[3] == 0.0) Direction = direction;
    else Direction = direction - Origin;
}