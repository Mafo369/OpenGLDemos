#ifndef LIGHT_H
#define LIGHT_H

#include "../Rendering/RenderObject.h"

typedef struct s_Attenuation
{
    float constant = 1;
    float linear = 0;
    float quadratic = 0;
}Attenuation;

typedef struct s_LightParams
{
    glm::vec3 position;
    glm::vec3 color; // Colour
    Attenuation attenuation;
}LightParams;

class Light : public RenderObject{
public:
    Light(LightParams lightParams);
    ~Light();

    void update(glm::vec3 translation);
    glm::mat4 getModel() { return m_lightmodel; }
    LightParams& getParams() { return m_lightParams; }
    glm::vec3& getLightPosition() { return m_lightParams.position; }
private:
    LightParams m_lightParams;
    glm::mat4 m_lightmodel;
};

#endif
