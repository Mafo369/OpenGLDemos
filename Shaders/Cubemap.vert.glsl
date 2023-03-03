#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inormal;
layout (location = 2) in vec2 itexCoords;
layout (location = 3) in vec4 in_color;

out vec3 localPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    localPos = position;  
    gl_Position =  projection * view * model * vec4(localPos, 1.0);
}
