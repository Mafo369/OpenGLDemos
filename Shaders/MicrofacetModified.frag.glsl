#version 410 core

#define PI 3.1415926535897932384626433832795

layout (location = 0) in vec4 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texCoords;
layout (location = 3) in vec4 in_color;
layout (location = 4) in vec3 eyePos;

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

float sqrtFun(float XdotY, float alpha2){
  float XdotY2 = XdotY * XdotY;
  return sqrt(alpha2 + (1-alpha2) * XdotY2);
}

float ggx(float NdotH, float alpha2){
  float f = (NdotH * alpha2 - NdotH) * NdotH + 1.0;
  return alpha2 / (PI * f * f);
}

float smith(float NdotL, float HdotL, float NdotV, float HdotV, float alpha2){
  return (( 2.0 * NdotL) / (NdotL + sqrtFun(NdotL, alpha2)) ) * ( (2.0 * NdotV ) / (NdotV + sqrtFun(NdotV, alpha2)));
}

void main(){
  
  const vec3 black = vec3(0);
  float ior = 0.04;
  vec3 iorV = vec3(ior);
  vec3 baseColor = in_color.rgb;
  float metallic = 0.6;
  float roughness = 0.6;
  vec3 n = gl_FrontFacing ? in_normal : -in_normal;

  vec3 v = normalize(eyePos - in_position.xyz);
  vec3 material = vec3(0.0);
  for(int i =0; i < 3; i++){
    vec3 l = normalize(light[i].position - in_position.xyz);
    vec3 h = normalize(l + v);
    float VdotH = clamp(dot(v, h), 0.0, 1.0);
    float NdotH = clamp(dot(n, h), 0.0, 1.0);
    float NdotL = clamp(dot(n, l), 0.001, 1.0);
    float HdotL = clamp(dot(h, l), 0.0, 1.0);
    float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);

    vec3 c_diff = mix(baseColor.rgb, black, metallic);
    vec3 f0 = mix(iorV, baseColor.rgb, metallic);
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;

    vec3 F = f0 + (1-f0) * pow(clamp(1.0 - VdotH, 0.0, 1.0),5);

    vec3 f_diffuse = (1 - F) * (1 / PI) * c_diff;
    float D = ggx(NdotH, alpha2);
    float G = smith(NdotL, HdotL, NdotV, VdotH, alpha2);
    vec3 f_specular = F * D * G / (4 * NdotV * NdotL);

    material += NdotL * (f_diffuse + f_specular);
  }
  material = clamp(material, 0.0, 1.0);

  color = vec4(material, 1.0);
}
