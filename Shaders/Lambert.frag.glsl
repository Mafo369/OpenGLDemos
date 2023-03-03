#version 460 core

in vec3 out_position;
in vec3 out_normal;
in vec2 out_texCoords;
in vec4 out_color;

layout (location = 0) uniform sampler2DArray shadowMap;
layout (location = 1) uniform samplerCube envMap;

out vec4 color;
uniform vec3 eyePosition;
uniform float farPlane;
uniform mat4 view;

struct DirLight
{
  vec3 direction;
  vec3 color;
};

uniform DirLight dirLight;

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

layout (std140, binding = 0) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[3];
};
uniform float cascadePlaneDistances[3];
uniform int cascadeCount;   // number of frusta - 1

float ShadowCalculation(vec3 fragPosWorldSpace, out vec3 color)
{
    // select cascade layer
    vec4 fragPosViewSpace = view * vec4(fragPosWorldSpace, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for (int i = 0; i < cascadeCount; ++i)
    {
        if (depthValue < cascadePlaneDistances[i])
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = cascadeCount;
    }

    color = vec3(0);
    color[layer] = 1;

    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPosWorldSpace, 1.0);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (currentDepth > 1.0)
    {
        return 0.0;
    }
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(out_normal);
    float bias = max(0.05 * (1.0 - dot(normal, dirLight.direction)), 0.005);
    const float biasModifier = 0.5f;
    if (layer == cascadeCount)
    {
        bias *= 1 / (farPlane * biasModifier);
    }
    else
    {
        bias *= 1 / (cascadePlaneDistances[layer] * biasModifier);
    }

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
        
    return shadow;
}

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
    vec3 levelColor = vec3(0);
    float shadow = ShadowCalculation(out_position.xyz, levelColor);                      
    vec3 I = lightContributionFrom(light[i], out_position.xyz);
    vec3 n = gl_FrontFacing ? out_normal : -out_normal;
    result += (1.0 - shadow) * (out_color * lambert(n, light[i].position) * vec4(I, 1.0));
  }

  color = result.rgba;
}
