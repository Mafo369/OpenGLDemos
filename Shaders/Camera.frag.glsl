#version 460 core
layout (location = 1) in vec3 in_normal;

out vec4 color;

void main()
{
    //color = vec4(vec3(clamp(dot(normalize(normal), vec3(0,0,1)), 0, 1)), 1.0);
    color = vec4(normalize(in_normal)*0.5+0.5, 1.0);
};
