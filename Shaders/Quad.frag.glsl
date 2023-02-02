#version 330 core
out vec4 FragColor;
  
in vec3 out_normal;
in vec4 out_color;
in vec2 out_texCoords;
uniform sampler2D screenTexture;

void main()
{ 
    FragColor = texture(screenTexture, out_texCoords);
    //FragColor = vec4(out_texCoords,0,1);
}
