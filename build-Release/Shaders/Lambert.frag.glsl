#version 410 core

layout (location = 0) in vec4 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texCoords;
layout (location = 3) in vec4 in_color;

out vec4 color;

struct Attenuation
{
  float constant;
  float linear;
  float quadratic;
};

struct Light
{
  vec3 position;
  vec3 color; // Colour
  Attenuation attenuation;
};

uniform Light light[3];

float lambert(vec3 N, vec3 L)
{
  vec3 nrmN = normalize(N);
  vec3 nrmL = normalize(L);
  float result = dot(nrmN, nrmL);
  return max(result, 0.0);
}

vec3 lightContributionFrom(Light light, vec3 position){
  float d = length(light.position - position);
  float attenuation = light.attenuation.constant + light.attenuation.linear * d +
                      light.attenuation.quadratic * d * d;
  return (1.f / attenuation) * light.color;
}

void main()
{
  vec4 result = vec4(0.0);
  for(int i =0; i < 3; i++){
    vec3 I = lightContributionFrom(light[i], in_position.xyz);
    vec3 n = gl_FrontFacing ? in_normal : -in_normal;
    result += in_color * lambert(n, light[i].position) * vec4(I, 1.0);
  }

  result = clamp(result, 0.0, 1.0);

  color = result.rgba;
}
