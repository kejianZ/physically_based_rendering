#pragma once
#include <glm/glm.hpp>
#include "Material.hpp"
using namespace glm;

class Record
{
public:
    bool hit = false;
    float min_t;        // the minimum t closest to origin
    vec4 normal;        // surface normal
    vec3 view;          // view direction
    vec3 light;         // light direction
    vec4 intersection;  // the intersection point
    Material *material;
    Record();
    void over_write(float new_t, vec4 n, vec4 inter, Material *m);
    void shadow_write();
};



