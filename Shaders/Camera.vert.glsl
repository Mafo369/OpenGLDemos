#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inormal;
layout (location = 2) in vec2 itexCoords;
layout (location = 3) in vec4 in_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 out_position;
out vec3 out_normal;
out vec2 out_texCoords;
out vec4 out_color;

void main()
{
    vec4 pos = projection * view * model * vec4(position, 1.0f);
    gl_Position = pos;
    out_normal = normalize(transpose(inverse(mat3(model))) * inormal);
    out_texCoords = itexCoords;
    out_position = vec3(model * vec4(position, 1.0));
    out_color = in_color;
};
