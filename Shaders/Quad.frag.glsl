#version 330 core
out vec4 FragColor;
  
in vec4 out_position;
in vec3 out_normal;
in vec2 out_texCoords;
in vec4 out_color;
uniform sampler2D screenTexture;
uniform float exposure;

void main()
{ 
    const float gamma = 2.2;
    vec3 hdrColor = texture(screenTexture, out_texCoords).rgb;
  
    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
  
    FragColor = vec4(mapped, 1.0);
}
