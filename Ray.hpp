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
    vec4 Origin;
    vec4 Direction;

    RayType Type;

    Ray(vec4 origin, vec4 direction, bool dir = true, RayType type = RayType::ViewRay);
    Ray();
    vec4 pos_at(float t);
    void transform(mat4 trans);
};
