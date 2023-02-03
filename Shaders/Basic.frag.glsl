#version 410 core

out vec4 color;

in vec3 out_normal;
in vec4 out_color;
in vec2 out_texCoords;
        
void main(){
  color = out_color.rgba;
};
