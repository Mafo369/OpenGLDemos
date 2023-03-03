#version 460 core
out vec4 FragColor;
  
in vec3 out_position;
in vec3 out_normal;
in vec2 out_texCoords;
in vec4 out_color;

uniform sampler2D screenTexture;
uniform float threshold;

void main()
{ 
    vec3 hdrColor = texture(screenTexture, out_texCoords).rgb;
    float brightness = dot(hdrColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > threshold){
        FragColor = vec4(hdrColor, 1.0);
    }
    else
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
