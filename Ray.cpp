#include "Ray.hpp"
using namespace glm;

Ray::Ray(vec3 origin, vec3 direction, bool dir)
{
    Origin = origin;
    if (dir) Direction = normalize(direction);
    else Direction = normalize(direction - Origin);
}

Ray::Ray() {}

vec3 Ray::pos_at(float t)
{
    return Origin + t * Direction;
}