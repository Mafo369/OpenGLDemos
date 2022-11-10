#version 410 core

in vec3 out_position;
in vec3 out_normal;
in vec2 out_texCoords;
in vec4 out_color;

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
    vec3 I = lightContributionFrom(light[i], out_position.xyz);
    vec3 n = gl_FrontFacing ? out_normal : -out_normal;
    result += out_color * lambert(n, light[i].position - out_position) * vec4(I, 1.0);
  }

  result = clamp(result, 0.0, 1.0);

  color = result.rgba;
}
