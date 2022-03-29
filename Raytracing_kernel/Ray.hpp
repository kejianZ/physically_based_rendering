#pragma once
#include <glm/glm.hpp>
using namespace glm;

enum class RayType {
	ViewRay,
    ShadowRay,
    Reflect
};

class Ray
{
public:
    vec4 Origin;
    vec4 Direction;

    vec4 Inv_dir;
    int sign[3];

    RayType Type;

    Ray(vec4 origin, vec4 direction, bool dir = true, RayType type = RayType::ViewRay);
    Ray();
    vec4 pos_at(float t);
    Ray transform(mat4 trans);
    void cal_inv();
};
