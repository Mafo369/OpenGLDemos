#version 410 core
        
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inormal;
layout (location = 2) in vec2 itexCoords;

out vec3 normal;
        
void main(){
  normal=inormal;
  gl_Position = vec4(position.x, position.y, position.z, 1.0);
};
