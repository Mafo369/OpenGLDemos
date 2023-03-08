#version 460 core

#define PI 3.1415926535897932384626433832795
#define MAX_LIGHTS 6

in vec3 out_position;
in vec3 out_normal;
in vec2 out_texCoords;
in vec4 out_color;
in vec3 out_tangent;
in vec3 out_bitangent;
in mat3 out_TBN;

layout (location = 0) uniform sampler2DArray shadowMap;
layout (location = 1) uniform samplerCube envMap;

struct Light
{
  vec3 position;
  vec3 color;
  float constant;
  float linear;
  float quadratic;
};

struct DirLight
{
  vec3 direction;
  vec3 color;
};

struct Material{
  sampler2D texDiffuse;
  sampler2D texSpecular;
  sampler2D texNormal;
  sampler2D texAO;
  sampler2D texEmission;
};

uniform vec3 eyePosition;
uniform int nLights;
uniform Light light[MAX_LIGHTS];
uniform DirLight dirLight;
uniform Material material;

out vec4 FragColor;


float chiplus(float c) { return (c > 0.f) ? 1.f : 0.f; }

float sqrtFun(float XdotY, float alpha2){
  float XdotY2 = XdotY * XdotY;
  return sqrt(alpha2 + (1-alpha2) * XdotY2);
}

float ggx(float NdotH, float alpha2){
  float NdotH2 = NdotH * NdotH;
  return (alpha2 * chiplus(NdotH)) / (PI  * pow((NdotH2*(alpha2 - 1) + 1), 2));
}

float smith(float NdotL, float HdotL, float NdotV, float HdotV, float alpha2){
  return ( 2 * abs(NdotL) * chiplus(HdotL) / (abs(NdotL) + sqrtFun(NdotL, alpha2)) ) * ( 2 * abs(NdotV) * chiplus(HdotV) / (abs(NdotV) + sqrtFun(NdotV, alpha2)));
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

void main(){
    vec3 albedo = pow(texture(material.texDiffuse, out_texCoords).rgb, vec3(2.2));

    vec4 texColorSpecular = texture(material.texSpecular, out_texCoords);
    float roughness = float(texColorSpecular.g);
    float metallic = float(texColorSpecular.b);
  
    vec3 normal = texture(material.texNormal, out_texCoords).rgb;
    vec3 N;
    if(normal.r == 0 && normal.g == 0 && normal.b == 0){
        N = out_normal;
    }
    else{
        normal = normal * 2.0 - 1.0;
        N = normalize(out_TBN * normal);
    }
    vec3 V = normalize(eyePosition - out_position);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
	           
    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < nLights; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(light[i].position - out_position);
        vec3 H = normalize(V + L);
        float distance    = length(light[i].position - out_position);
        float attenuation = 1.0 / (distance);
        vec3 radiance     = light[i].color * attenuation;        
        
        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, roughness);        
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;  
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);                
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
    }   

    vec3 Le = 3 * pow(texture(material.texEmission, out_texCoords).rgb, vec3(2.2));
  
    vec3 ao = texture(material.texAO, out_texCoords).rgb;
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = Le + ambient + Lo;
   
    color = clamp(color, 0, 1000000000);
    FragColor = vec4(color, 1.0);


  //const vec3 black = vec3(0);
  //vec3 iorV = vec3(0.04);

  //vec4 texColor = texture(material.texDiffuse, out_texCoords);
  //float gamma = 2.2;
  //vec3 diffuseColor = pow(texColor.rgb, vec3(gamma));
  //texColor = vec4(diffuseColor, 1.0);
  //vec4 texColorSpecular = texture(material.texSpecular, out_texCoords);
  //vec3 baseColor = texColor.rgb;
  //float roughness = float(texColorSpecular.g);
  //float metallic = float(texColorSpecular.b);

  ////metallic = 0.9;
  ////roughness = 0.2;

  //vec3 f0 = mix(iorV, baseColor.rgb, metallic);
  //vec3 c_diff = mix(baseColor.rgb, black, metallic);
  //float alpha = roughness * roughness;
  //float alpha2 = alpha * alpha;

  //vec3 n = gl_FrontFacing ? out_normal : -out_normal;
  //n = normalize(n);

  //vec3 v = normalize(eyePosition - out_position.xyz);
  //vec3 material = vec3(0.0);
  //for(int i =0; i < nLights; i++){
  //  vec3 l = normalize(light[i].position - out_position.xyz);
  //  vec3 h = normalize(l + v);
  //  float VdotH = dot(v, h);
  //  float NdotH = dot(n, h);
  //  float NdotL = dot(n, l);
  //  float HdotL = dot(h, l);
  //  float NdotV = dot(n, v);

  //  vec3 F = f0 + (1-f0) * pow(1 - abs(VdotH),5);

  //  vec3 f_diffuse = (1 - F) * (1 / PI) * c_diff;
  //  float D = ggx(NdotH, alpha2);
  //  float G = smith(NdotL, HdotL, NdotV, VdotH, alpha2);
  //  vec3 f_specular = F * D * G / (4 * abs(NdotV) * abs(NdotL));

  //  float distance = length(out_position.xyz - light[i].position);
  //  float attenuation = 1.0 / distance;  

  //  material += attenuation * light[i].color * (f_diffuse + f_specular) * max(NdotL, 0.0);
  //}


  //material = clamp(material, 0, 1000000000);
  //FragColor = vec4(material, 1.0);


    ///vec3 color = texture(material.texDiffuse, out_texCoords).rgb;
    ///vec3 normal = normalize(out_normal);
    ///// ambient
    ///vec3 ambient = 0.0 * color;
    ///// lighting
    ///vec3 lighting = vec3(0.0);
    ///vec3 viewDir = normalize(eyePosition - out_position.xyz);
    ///for(int i = 0; i < 6; i++)
    ///{
    ///    // diffuse
    ///    vec3 lightDir = normalize(light[i].position - out_position.xyz);
    ///    float diff = max(dot(lightDir, normal), 0.0);
    ///    vec3 result = light[i].color * diff * color;      
    ///    // attenuation (use quadratic as we have gamma correction)
    ///    float distance = length(out_position.xyz - light[i].position);
    ///    result *= 1.0 / (distance * distance);
    ///    lighting += result;
    ///            
    ///}
    ///vec3 result = ambient + lighting;
    ///FragColor = vec4(result, 1.0);
}
