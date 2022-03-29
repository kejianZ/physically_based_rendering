#pragma once
#include <glm/glm.hpp>
#include "../Object_attributes/Material.hpp"
using namespace glm;

class Record
{
public:
    bool hit;
    float min_t;        // the minimum t closest to origin
    vec4 normal;        // surface normal
    vec4 intersection;  // the intersection point
    uint hit_node = UINT_MAX;  // -1 means hit background
    Material *material;
    Record();
    bool over_write(float new_t, vec4 n, vec4 inter, Material *m);
    void shadow_write();
    double texture_x;
    double texture_y;
};



