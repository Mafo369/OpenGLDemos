#version 460 core
        
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inormal;
layout (location = 2) in vec2 itexCoords;
layout (location = 3) in vec4 in_color;

out vec3 out_normal;
out vec4 out_color;
out vec2 out_texCoords;
        
void main(){
  out_normal=inormal;
  out_color = in_color;
  out_texCoords = itexCoords;
  gl_Position = vec4(position.x, position.y, 0, 1.0);
};
