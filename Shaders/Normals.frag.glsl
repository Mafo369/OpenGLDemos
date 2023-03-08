#version 460 core
    
in vec3 out_position;
in vec3 out_normal;
in vec2 out_texCoords;
in vec4 out_color;

out vec4 color;

void main(){
  color = vec4((normalize(out_normal)+1)*0.5, 1.0f);
};
