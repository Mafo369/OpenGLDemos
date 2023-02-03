#version 410 core
    
in vec4 out_position;
in vec3 out_normal;
in vec2 out_texCoords;
in vec4 out_color;

out vec4 color;

void main(){
  color = vec4(out_texCoords.xy, 1 - (out_texCoords.x + out_texCoords.y) / 2, 1.0f);
};
