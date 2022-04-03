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
using namespace std;

class Rasterization
{
private:
    GLFWwindow * m_window;
    vector<GLuint> buffers;
    vector<int> counts;
    GLuint shader_ID;
    int accumulative_patchID = 0;
public:
    Rasterization();
    ~Rasterization();
    void add_patch(float * vertexs, bool *vert_mask, int *index, int vertex_len, int index_len);
    void display();
};

