#version 410 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inormal;
layout (location = 2) in vec2 itexCoords;
layout (location = 3) in vec4 in_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
layout (location = 0) out vec4 out_position;
layout (location = 1) out vec3 out_normal;
layout (location = 2) out vec2 out_texCoords;
layout (location = 3) out vec4 out_color;

void main()
{
    vec4 pos = projection * view * model * vec4(position, 1.0f);
    gl_Position = pos;
    out_normal = inormal;
    out_texCoords = itexCoords;
    out_position = model * vec4(position, 1.0);
    out_color = in_color;
};
