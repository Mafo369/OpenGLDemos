#ifndef LIGHT_H
#define LIGHT_H

#include "../Rendering/RenderObject.h"

typedef struct s_PointLightParams
{
    glm::vec3 position;
    glm::vec3 color;

    float constant = 1;
    float linear = 0;
    float quadratic = 0;
} PointLightParams;

typedef struct s_DirLightParams
{
  glm::vec3 direction;
  glm::vec3 color;
} DirLightParams;

class PointLight : public RenderObject{
public:
    PointLight(PointLightParams lightParams);
    ~PointLight();

    void update(glm::vec3 translation);
    glm::mat4 getModel() { return m_lightmodel; }
    PointLightParams& getParams() { return m_lightParams; }
    glm::vec3& getLightPosition() { return m_lightParams.position; }
private:
    PointLightParams m_lightParams;
    glm::mat4 m_lightmodel;
};

#endif
