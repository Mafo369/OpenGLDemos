#version 410 core
    
in vec2 out_texCoords;

out vec4 color;

void main(){
  color = vec4(out_texCoords.xy, 1 - (out_texCoords.x + out_texCoords.y) / 2, 1.0f);
};
