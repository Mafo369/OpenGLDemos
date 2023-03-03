#version 460 core
out vec4 FragColor;

in vec3 out_position;
in vec3 out_normal;
in vec2 out_texCoords;
in vec4 out_color;

layout (location = 0) uniform sampler2DArray shadowMap;
layout (location = 1) uniform samplerCube envMap;

struct DirLight
{
  vec3 direction;
  vec3 color;
};

uniform DirLight dirLight;

uniform vec3 eyePosition;
uniform float farPlane;

uniform mat4 view;

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

void main()
{           
    vec3 normal = out_normal;
    vec3 viewDir = normalize(eyePosition - out_position.xyz);
    vec3 reflectDir = reflect(-viewDir, normal);
    vec3 color = texture(envMap, reflectDir).rgb;
    vec3 lightColor = dirLight.color;
    vec3 lightDir = dirLight.direction;
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    viewDir = normalize(eyePosition - out_position.xyz);
    reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    vec3 levelColor;
    float shadow = ShadowCalculation(out_position.xyz, levelColor);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    //FragColor = vec4(lighting, 1.0);
    //FragColor = vec4(levelColor, 1.0);
    FragColor = vec4(color, 1.0);
}
