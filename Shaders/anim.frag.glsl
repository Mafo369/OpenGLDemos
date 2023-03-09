#version 460 core

out vec4 FragColor;

in vec3 out_normal;
in vec4 out_color;
in vec2 out_texCoords;

void main()
{
    float gamma = 2.2;
    vec3 baseColor = vec3(out_texCoords.x, out_texCoords.y, 0);
    baseColor = pow(baseColor, vec3(gamma));
    FragColor = vec4(baseColor, 1);
}
