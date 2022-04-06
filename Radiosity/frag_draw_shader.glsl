#version 330 core

in vec3 v_color;
out vec3 frag_color;
void main(){
  frag_color = v_color;
}