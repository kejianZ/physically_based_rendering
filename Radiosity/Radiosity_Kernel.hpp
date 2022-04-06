#pragma once
#include <iostream>
#include <vector>

#include "Rasterization_Kernel.hpp"
#include "../Raytracing_kernel/Frame.hpp"
#include "../Object_attributes/Primitive.hpp"
class Primitive;

class Radiosity_Kernel
{
    struct patchlized_primitive
    {
        int patch_count;
        vec4 *patch_origins;
        Frame *patch_coordinates;
        int (*mapping)(int);
        vec3 reflectance;
        vec3 emission;
        Primitive *origin_primitive;
    };

    // form factor - energy leaving patch i and reached patch j
    struct p_ij
    {
        int patch_j_id;
        float form_factor;
    };

    struct patch
    {
        p_ij* form_factors;
        int ff_len;
        vec3 incident;
        vec3 excident;
        vec3 *reflectance;
        vec3 *emission;
        patch(vec3 *refl, vec3 *emis)
        {
            reflectance = refl;
            emission = emis;
            excident = *emis;
        }
        patch() {}
    };
private:
    Rasterization *z_buffer_tool;
    vector<patchlized_primitive> primitives;
    int z_buffer_size = 800;
    int total_patch_count = 0;
    patch* patches;
    vector<bool *> color_maps;
    vector<int> vertex_counts;
public:
    Radiosity_Kernel();
    ~Radiosity_Kernel();

    void primitive_add_patch(int patch_count, vec4 *patch_origins, Frame *patch_coordinates, int (*mapping)(int), vec3 reflectance, vec3 emission, Primitive *p);
    void primitive_feed_opengl(float * vertexs, bool *vert_mask, int *index, int vertex_len, int index_len);
    void display();
    float cal_correction_term(int face_no, int index);
    void patches_init();
    void hemicube();
    void radiosity_main_loop(int recursive_time);
    void write_patch_color();
    void draw_scene();
};
