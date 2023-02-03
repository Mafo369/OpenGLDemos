#pragma once

#include "Mesh.h"
#include "Light.h"

class Cube : public Mesh {
public:
    Cube(glm::vec3 position, glm::vec4 color);
    ~Cube();

private:
    glm::vec3 m_position;
    glm::vec4 m_color;
};
