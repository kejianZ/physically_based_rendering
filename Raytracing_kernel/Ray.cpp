#include "Ray.hpp"
using namespace glm;

Ray::Ray(vec4 origin, vec4 direction, bool dir, RayType type)
{
    Type = type;
    Origin = origin;
    if (dir) Direction = direction;
    else Direction = normalize(direction - Origin);
}

Ray::Ray() {}

vec4 Ray::pos_at(float t)
{
    return Origin + t * Direction;
}

Ray Ray::transform(mat4 trans)
{
    return Ray(trans * Origin, trans * Direction, true, this->Type);
}

void Ray::cal_inv()
{
    Inv_dir = vec4(1/Direction[0], 1/Direction[1], 1/Direction[2], 0);
    sign[0] = Inv_dir[0] < 0;
    sign[1] = Inv_dir[1] < 0;
    sign[2] = Inv_dir[2] < 0;
}