#include "Radiosity_Kernel.hpp"
#include <unordered_map>
#include <cmath>
#include <glm/ext.hpp>
#include <memory>
#include "../IO_interface/Image.hpp"
using namespace glm;
const int max_id = 16777215;
const float m_pi=3.14159265358979f;

Radiosity_Kernel::Radiosity_Kernel()
{
    z_buffer_tool = new Rasterization();
    z_buffer_tool->generate_window(false, "Radiosity/vertex_shader.glsl", "Radiosity/fragment_shader.glsl");
}

Radiosity_Kernel::~Radiosity_Kernel()
{
}

void Radiosity_Kernel::display()
{
    patches_init();
    cout << total_patch_count << endl;
    hemicube();
    cout << "finish form factor calculation" << endl;
    radiosity_main_loop(20);
    cout << "finish calculation" << endl;
    draw_scene();
    // write_patch_color();
    // Image image(800, 800);
    // u_char* img = z_buffer_tool->patch_form_factor(vec3(0,5,4), vec3(0,0,-1), vec3(0,1,0));
    // for(int i = 0; i < 640000; i++)
    // {
    //     double channel = 255;
    //     image(i % 800, 799 - i / 800, 0) = (double)img[3*i] / channel;
    //     image(i % 800, 799 - i / 800, 1) = (double)img[3*i+1] / channel;
    //     image(i % 800, 799 - i / 800, 2) = (double)img[3*i+2] / channel;
    // }
    // image.savePng("rd.png");
}

void Radiosity_Kernel::draw_scene()
{
    Rasterization *dis = new Rasterization();
    dis->generate_window(true, "Radiosity/vert_draw_shader.glsl", "Radiosity/frag_draw_shader.glsl");
    //dis->generate_window(false, "Radiosity/vertex_shader.glsl", "Radiosity/fragment_shader.glsl");
    int cumulative_id = 0;
    for(patchlized_primitive p: primitives)
    {
        vec3 colors[p.patch_count];
        for(int i = 0; i < p.patch_count; i++)
        {
            colors[i] = patches[cumulative_id++].excident;
        }
        p.origin_primitive->draw_primitive(dis, colors);
    }

    dis->display();
    // Image image(800, 800);
    // u_char* img = dis->patch_form_factor(vec3(0,5,4), vec3(0,0,-1), vec3(0,1,0));
    // for(int i = 0; i < 640000; i++)
    // {
    //     double channel = 255;
    //     image(i % 800, 799 - i / 800, 0) = (double)img[3*i] / channel;
    //     image(i % 800, 799 - i / 800, 1) = (double)img[3*i+1] / channel;
    //     image(i % 800, 799 - i / 800, 2) = (double)img[3*i+2] / channel;
    // }
    // image.savePng("rd.png");
}

vec3 element_multiply(vec3 v1, vec3 v2)
{
    return vec3(v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2]);
}

void Radiosity_Kernel::radiosity_main_loop(int recursive_time)
{
    for(int time = 0; time < recursive_time; time++)
    {
        for(int i = 0; i < total_patch_count; i++)
        {
            patch &p = patches[i];
            for(int j = 0; j < p.ff_len; j++)
            {
                int j_id = p.form_factors[j].patch_j_id;    
                float ff = p.form_factors[j].form_factor;
                p.incident += ff * patches[j_id].excident;
            }
            //cout << "calculating patch " << i << " incident: " << glm::to_string(p.incident) << endl;
        }

        for(int i = 0; i < total_patch_count; i++)
        {
            patch &p = patches[i];
            p.excident = (*p.emission) + element_multiply(p.incident, (*p.reflectance)); 
            //cout << "calculating patch " << i << " excident: " << glm::to_string(*p.emission) << ' ' << glm::to_string(p.incident) << ' ' << glm::to_string(*p.reflectance) << ' ' << glm::to_string(p.excident) << endl;
            p.incident = vec3(0,0,0);
        }
    }
}

void Radiosity_Kernel::write_patch_color()
{
    int cumulative_id = 0;
    for(int i = 0; i < color_maps.size(); i++)
    {
        bool* color_map = color_maps[i];
        float colors[vertex_counts[i] * 3];
        for(int v = 0; v < vertex_counts[i]; v++)
        {
            //cout << i << ' ' << v << ' ' << int(color_map[v]) << endl;
            if(color_map[v])
            {
                vec3 patch_color = patches[cumulative_id++].excident;
                //cout << glm::to_string(patch_color) << endl;
                colors[v * 3] = patch_color[0];
                colors[v * 3 + 1] = patch_color[1];
                colors[v * 3 + 2] = patch_color[2];
            }
        }
        z_buffer_tool->rewrite_color(i, colors, vertex_counts[i]);
    }
}

// init all the patches in the scene
void Radiosity_Kernel::patches_init()
{
    patches = new patch[total_patch_count];
    int cumulative_id = 0;
    for(patchlized_primitive primitive:primitives)
    {
        vec3* reflect = new vec3();
        vec3* emis = new vec3();
        *reflect = primitive.reflectance;
        *emis = primitive.emission;
        for(int i = 0; i < primitive.patch_count; i++)
        {
            patches[cumulative_id++] = patch(reflect, emis);
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

    // the correction term should also divides pi, however, our reflectance is in the range [0,1] where by the definition of reflectance its range should be [0,1/pi], they cancel out each other
    float divides = m_pi * pow(pow(x,2) + pow(y,2) + pow(z,2), 2);
    return face_no == 0? z/divides: y/divides;
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
                    if(patch_id == max_id) continue;

                    if(patch_ff.find(patch_id) == patch_ff.end()) 
                        patch_ff[patch_id] = cal_correction_term(face_no, i) / pow(z_buffer_size, 2) * 4;
                    else
                        patch_ff[patch_id] += cal_correction_term(face_no, i) / pow(z_buffer_size, 2) * 4;
                }
                delete [] img;
            }
            p_ij* patch_ff_buffer = new p_ij[patch_ff.size()];
            int ind = 0;
            for(auto pair: patch_ff)
            {
                patch_ff_buffer[ind++] = p_ij{pair.first, pair.second};
            }
            patches[cumulative_id].form_factors = patch_ff_buffer;
            patches[cumulative_id++].ff_len = patch_ff.size();
        }
        cout << "Finish: " << cumulative_id << " patch out of " << total_patch_count << endl;
    }
}

//Store patch info on CPU side to calculate form factor
//It's possible that many patch shares the same coordinates system, we use mapping to map patchID to coordinates array
void Radiosity_Kernel::primitive_add_patch(int patch_count, vec4 *patch_origins, Frame *patch_coordinates, int (*mapping)(int), vec3 reflectance, vec3 emission, Primitive* p)
{
    // need frame for every patch, can be share within whole object
    // need origin for every patch
    // need reflectance for every patch
    // need record of how many patch in the object
    total_patch_count += patch_count;
    primitives.push_back(Radiosity_Kernel::patchlized_primitive{patch_count, patch_origins, patch_coordinates, mapping, reflectance, emission, p});
}

//Store patch info on GPU side to draw primitives
void Radiosity_Kernel::primitive_feed_opengl(float * vertexs, bool *vert_mask, int *index, int vertex_len, int index_len)
{
    color_maps.push_back(vert_mask);
    vertex_counts.push_back(vertex_len);
    z_buffer_tool->add_patch(vertexs, vert_mask, NULL, index, vertex_len, index_len);
}
