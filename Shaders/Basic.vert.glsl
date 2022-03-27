#version 410 core
        
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inormal;
layout (location = 2) in vec2 itexCoords;
layout (location = 3) in vec4 in_color;

layout (location = 1) out vec3 normal;
layout (location = 3) out vec4 out_color;
        
void main(){
  normal=inormal;
  out_color = in_color;
  gl_Position = vec4(position.x, position.y, position.z, 1.0);
};
