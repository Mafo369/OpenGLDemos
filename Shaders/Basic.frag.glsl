#version 410 core

layout (location = 1) in vec3 normal;
layout (location = 3) in vec4 in_color;

out vec4 color;
        
void main(){
  color = in_color.rgba;
};
