#include "Radiosity_Kernel.hpp"
#include <unordered_map>
#include <cmath>
#include <glm/ext.hpp>
using namespace glm;
const int max_id = 16777215;
const float m_pi=3.14159265358979f;

Radiosity_Kernel::Radiosity_Kernel()
{
    z_buffer_tool = new Rasterization();
}

Radiosity_Kernel::~Radiosity_Kernel()
{
}

void Radiosity_Kernel::display()
{
    //z_buffer_tool->display();
    ff_buffer_init();
    hemicube();
    for(int i = 0; i <= total_patch_count; i++)
    {
        int idx = 0;
        while(ff_buffer[i][idx].patch_j_id != -1)
        {
            cout << ff_buffer[i][idx].patch_j_id << ' ' << ff_buffer[i][idx].form_factor << endl;
            idx++;
        }
    }
}

float Radiosity_Kernel::cal_correction_term(int face_no, int index)
{
    int pix_x = index % z_buffer_size;
    int pix_y = index / z_buffer_size;
    float x = (pix_x + 0.5) * (2.0f / z_buffer_size) - 1;
    float y = (pix_y + 0.5) * (2.0f / z_buffer_size) - 1;
    float z = 1;
    //cout << x << ' ' << y << ' ' << z << endl;
    float divides = m_pi * pow(pow(x,2) + pow(y,2) + pow(z,2), 2);
    return face_no == 0? z/divides: y/divides;
}

// init the buffer to hold form factor for all patches
void Radiosity_Kernel::ff_buffer_init()
{
    for(patchlized_primitive p: primitives) total_patch_count += p.patch_count;
    ff_buffer = new p_ij*[total_patch_count];
}

void Radiosity_Kernel::hemicube()
{
    int cumulative_id = 0;
    // for each primitive in the scene
    for(patchlized_primitive p: primitives)
    {
        // for each patch attached on the primitive apply hemicube algorithm
        for(int p_no = 0; p_no < p.patch_count; p_no++)
        {
            unordered_map<int, float> patch_ff;
            Frame patch_frame = p.patch_coordinates[p.mapping(p_no)];

            // a hemicube has 5 faces, calculate each face seperately
            for(int face_no = 0; face_no < 5; face_no++)
            {   
                // calculate the rasterization view matrix
                vec3 view_dir, up;
                if(face_no == 0) {view_dir = vec3(patch_frame.y); up = vec3(patch_frame.x);}
                else if(face_no == 1) {view_dir = vec3(patch_frame.x); up = vec3(patch_frame.y);}
                else if(face_no == 2) {view_dir = vec3(-patch_frame.x); up = vec3(patch_frame.y);}
                else if(face_no == 3) {view_dir = vec3(patch_frame.z); up = vec3(patch_frame.y);}
                else {view_dir = vec3(-patch_frame.z); up = vec3(patch_frame.y);}

                // catch renderred frame from one of the patch's hemicube face
                u_char* img = z_buffer_tool->patch_form_factor(vec3(p.patch_origins[p_no]) + 0.01f * view_dir, view_dir, up);
                int pix_begin = face_no == 0? 0:pow(z_buffer_size, 2) / 2;
                for(int i = pix_begin; i < pow(z_buffer_size, 2); i++)
                {
                    int patch_id = img[i*3] + img[i*3+1] * 256 + img[i*3+2] * 65536;
                    //if(patch_id == max_id) continue;

                    if(patch_ff.find(patch_id) == patch_ff.end()) 
                        patch_ff[patch_id] = cal_correction_term(face_no, i) / pow(z_buffer_size, 2) * 4;
                    else
                        patch_ff[patch_id] += cal_correction_term(face_no, i) / pow(z_buffer_size, 2) * 4;
                }
                delete img;
            }
            p_ij* patch_ff_buffer = new p_ij[patch_ff.size()+1];
            int ind = 0;
            float sum = 0;
            for(auto pair: patch_ff)
            {
                patch_ff_buffer[ind++] = p_ij{pair.first, pair.second};
                //cout << pair.first  << ' '<< pair.second << endl;
                sum += pair.second;
            }
            cout << sum << endl;
            patch_ff_buffer[patch_ff.size()] = p_ij{-1, 0};
            ff_buffer[cumulative_id++] = patch_ff_buffer;
        }
    }
}

//Store patch info on CPU side to calculate form factor
//It's possible that many patch shares the same coordinates system, we use mapping to map patchID to coordinates array
void Radiosity_Kernel::primitive_add_patch(int patch_count, vec4 *patch_origins, Frame *patch_coordinates, int (*mapping)(int),  vec3 reflectance)
{
    // need frame for every patch, can be share within whole object
    // need origin for every patch
    // need reflectance for every patch
    // need record of how many patch in the object
    primitives.push_back(Radiosity_Kernel::patchlized_primitive{patch_count, patch_origins, patch_coordinates, mapping, reflectance});
}

//Store patch info on GPU side to draw primitives
void Radiosity_Kernel::primitive_feed_opengl(float * vertexs, bool *vert_mask, int *index, int vertex_len, int index_len)
{
    z_buffer_tool->add_patch(vertexs, vert_mask, index, vertex_len, index_len);
}
