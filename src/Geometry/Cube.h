#pragma once

#include "Mesh.h"
#include "Light.h"

class Cube : public Mesh {
public:
    Cube(glm::vec3 position);
    ~Cube();

private:
    glm::vec3 m_position;
    glm::vec4 m_color;
};
