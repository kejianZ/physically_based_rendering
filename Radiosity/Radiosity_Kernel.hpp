#pragma once
#include <iostream>
#include <vector>

#include "Rasterization_Kernel.hpp"
#include "../Raytracing_kernel/Frame.hpp"

class Radiosity_Kernel
{
    struct patchlized_primitive
    {
        int patch_count;
        vec4 *patch_origins;
        Frame *patch_coordinates;
        int (*mapping)(int);
        vec3 reflectance;
    };

    // form factor - energy leaving patch i and reached patch j
    struct p_ij
    {
        int patch_j_id;
        float form_factor;
    };
private:
    Rasterization *z_buffer_tool;
    vector<patchlized_primitive> primitives;
    int z_buffer_size = 800;
    p_ij** ff_buffer;
    int total_patch_count = 0;
public:
    Radiosity_Kernel();
    ~Radiosity_Kernel();

    void primitive_add_patch(int patch_count, vec4 *patch_origins, Frame *patch_coordinates, int (*mapping)(int),  vec3 reflectance);
    void primitive_feed_opengl(float * vertexs, bool *vert_mask, int *index, int vertex_len, int index_len);
    void display();
    float cal_correction_term(int face_no, int index);
    void ff_buffer_init();
    void hemicube();
};
