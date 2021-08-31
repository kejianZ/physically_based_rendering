#include "Ray.hpp"
using namespace glm;

Ray::Ray(vec4 origin, vec4 direction, bool dir, RayType type)
{
    Type = type;
    Origin = origin;
    if (dir) Direction = normalize(direction);
    else Direction = normalize(direction - Origin);
}

Ray::Ray() {}

vec4 Ray::pos_at(float t)
{
    return Origin + t * Direction;
}

// vec3 Ray::transform(mat4 trans)
// {
//     Origin = vec3(trans * vec4(Origin, 1));
// }