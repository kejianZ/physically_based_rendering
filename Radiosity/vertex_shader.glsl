#version 330 core
in vec3 vertexPosition_modelspace;
in vec3 color;

uniform mat4 MVP;

flat out vec3 v_color;
void main(){
    v_color = color;
    gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
}