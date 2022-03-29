#pragma once

#include "Geometry/Mesh.h"
#include "Light.h"

class LightCube : public Mesh {
public:
    LightCube(glm::vec3 lightPosition);
    ~LightCube();

private:
    glm::vec3 m_lightPosition;
    glm::vec4 m_color;
};
