#version 460 core
out vec4 FragColor;
  
in vec3 out_position;
in vec3 out_normal;
in vec2 out_texCoords;
in vec4 out_color;

uniform sampler2D screenTexture;
uniform sampler2D origTexture;
uniform float exposure;

uniform float bloomStrength = 0.04f;

void main()
{ 
    vec3 hdrColor = texture(screenTexture, out_texCoords).rgb;
    vec3 origColor = texture(origTexture, out_texCoords).rgb;
    vec3 result = mix(origColor, hdrColor, bloomStrength); // linear interpolation

    result = vec3(1.0) - exp(-result * exposure);
    const float gamma = 2.2;
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);


    //const float gamma = 2.2;
    //vec3 hdrColor = texture(screenTexture, out_texCoords).rgb;
    //vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    //mapped = pow(mapped, vec3(1.0 / gamma));
  
    //vec3 origColor = texture(origTexture, out_texCoords).rgb;
    //mapped = mix(origColor, mapped, bloomStrength); // linear interpolation
    ////mapped = mapped + texture(origTexture, out_texCoords).rgb;
  
    //FragColor = vec4(mapped, 1.0);
}
