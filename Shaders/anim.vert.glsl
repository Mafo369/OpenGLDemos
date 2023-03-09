#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 inormal;
layout (location = 2) in vec2 itexCoords;
layout (location = 3) in vec4 in_color;
layout (location = 4) in vec3 in_tangent;
layout (location = 5) in vec3 in_bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 out_position;
out vec3 out_normal;
out vec2 out_texCoords;
out vec4 out_color;
out vec3 out_tangent;
out vec3 out_bitangent;
out mat3 out_TBN;

uniform mat4 bone1 = mat4(1);
uniform mat4 bone2 = mat4(1);

void main()
{
    mat4 blend = itexCoords.x * bone1 + itexCoords.y * bone2; 
    vec3 new_pos = vec3(blend * vec4(position,1));
    gl_Position = projection * view * model * vec4(new_pos, 1.0);
    out_texCoords = itexCoords;
    out_position = vec3(model * vec4(new_pos, 1.0));;
    out_color = in_color;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * in_tangent);
    vec3 N = normalize(normalMatrix * inormal);
    out_normal = N;
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    out_TBN = mat3(T, B, N);
};
