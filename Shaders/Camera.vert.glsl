#version 410 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inormal;
layout (location = 2) in vec2 itexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 normal;
out vec2 texCoords;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    normal = inormal;
    texCoords = itexCoords;
};
