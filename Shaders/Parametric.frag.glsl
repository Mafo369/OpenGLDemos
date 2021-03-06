#version 410 core
    
layout (location = 2) in vec2 texCoords;

out vec4 color;

void main(){
  color = vec4(texCoords.xy, 1 - (texCoords.x + texCoords.y) / 2, 1.0f);
};
