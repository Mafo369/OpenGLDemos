#version 460 core
out vec4 FragColor;

in vec3 out_normal;
in vec4 out_color;
in vec2 out_texCoords;

void main()
{
    FragColor = out_color;
}
