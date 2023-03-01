#pragma once

#include "Mesh.h"
#include "Light.h"

class Cube : public Mesh {
public:
    Cube(glm::vec4 color);
    ~Cube();

private:
    glm::vec4 m_color;
};
