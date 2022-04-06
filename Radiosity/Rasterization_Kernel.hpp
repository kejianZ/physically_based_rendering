#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#pragma once
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include "cs488-framework/OpenGLImport.hpp"
#include <GLFW/glfw3.h>
#include "../Raytracing_kernel/Frame.hpp"
using namespace std;

class Rasterization
{
private:
    GLFWwindow * m_window;
    vector<GLuint> buffers;
    vector<int> counts;
    GLuint shader_ID;
    int accumulative_patchID = 0;
    int img_size = 800;
public:
    Rasterization();
    ~Rasterization();
    void add_patch(float *vertexs, bool *vert_mask, float *vert_colors, int *index, int vertex_len, int index_len, int mode=0);
    void display();
    u_char* patch_form_factor(vec3 patch_origin, vec3 view_dir, vec3 up);
    void rewrite_color(int vao_no, float *colors, int len);
    void generate_window(bool visible, const string vertex_shader, const string fragment_shader);
};

