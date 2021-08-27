#pragma once
#include <glm/glm.hpp>
using namespace glm;

enum class RayType {
	ViewRay,
    ShadowRay
};

class Ray
{
public:
    vec3 Origin;
    vec3 Direction;

    RayType Type;

    Ray(vec3 origin, vec3 direction, bool dir = true, RayType type = RayType::ViewRay);
    Ray();
    vec3 pos_at(float t);
};
