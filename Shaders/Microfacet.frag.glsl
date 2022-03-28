#version 410 core

#define PI 3.1415926535897932384626433832795

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

uniform vec3 u_eye;
//uniform Light light;

float chiplus(float c) { return (c > 0.f) ? 1.f : 0.f; }

float sqrtFun(float XdotY, float alpha2){
  float XdotY2 = XdotY * XdotY;
  return sqrt(alpha2 + (1-alpha2) * XdotY2);
}

float ggx(float NdotH, float alpha2){
  float NdotH2 = NdotH * NdotH;
  return (alpha2 * chiplus(NdotH)) / (PI  * (NdotH2*(alpha2 - 1) + 1));
}

float specular_brdf(float alpha, float HdotL, float NdotL, float HdotV, float NdotV, float NdotH){
  float alpha2 = alpha*alpha;
  float V = (chiplus(HdotL) / (NdotL + sqrtFun(NdotL, alpha2)) ) * (chiplus(HdotV) / (NdotV + sqrtFun(NdotV, alpha2)));
  float D = ggx(NdotH, alpha2); 
  return V * D;
}

vec3 diffuse_brdf(vec3 col){
  return (1/PI) * col;
}

float smith(float NdotL, float HdotL, float NdotV, float HdotV, float alpha2){
  return ( 2 * NdotL * chiplus(HdotL) / (NdotL + sqrtFun(NdotL, alpha2)) ) * ( 2 * NdotV * chiplus(HdotV) / (NdotV + sqrtFun(NdotV, alpha2)));
}

//vec3 mix(vec3 dielectric_brdf, vec3 metal_brdf, float metallic){
//  return (1.0 - metallic) * dielectric_brdf + metallic * metal_brdf;
//}

void main(){
  //float specular = specular_brdf(roughness*roughness, HdotL, NdotL, HdotV, NdotV, NdotH);
  //vec3 dielectric_brdf = fresnel_mix(ior, diffuse_brdf(baseColor), specularF);
  //vec3 metal_brdf = conductor_fresnel(baseColor, specularF);
  //vec3 material = mix(dielectric_brdf, metal_brdf, metallic)
  
  const vec3 black = vec3(0);
  float ior = 0.04;
  vec3 iorV = vec3(ior);
  vec3 baseColor = in_color.rgb;
  float metallic = 0.1;
  float roughness = 1;
  vec3 lpos = vec3(2,1,4);

  vec3 v = normalize(u_eye - in_position.xyz);
  vec3 l = normalize(lpos - in_position.xyz);
  vec3 h = normalize(l + v);
  float VdotH = dot(v, h);
  float NdotH = dot(in_normal, h);
  float NdotL = dot(in_normal, l);
  float HdotL = dot(h, l);
  float NdotV = dot(in_normal, v);
  float HdotV = dot(h, v);
  float VdotN = dot(v, in_normal);
  float LdotN = dot(l, in_normal);


  vec3 c_diff = mix(baseColor.rgb, black, metallic);
  vec3 f0 = mix(iorV, baseColor.rgb, metallic);
  float alpha = roughness * roughness;
  float alpha2 = alpha * alpha;

  vec3 F = f0 + (1-f0) * pow(1 - abs(VdotH),5);

  vec3 f_diffuse = (1 - F) * (1 / PI) * c_diff;
  float D = ggx(NdotH, alpha2);
  float G = smith(NdotL, HdotL, NdotV, HdotV, alpha2);
  vec3 f_specular = F * D * G / (4 * abs(VdotN) * abs(LdotN));

  vec3 material = f_diffuse + f_specular;
  color = vec4(material, 1.0);
}
