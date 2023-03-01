#pragma once

#include "Mesh.h"

class Plane : public Mesh {
public:
    Plane(glm::vec4 color);
    ~Plane();

private:
    glm::vec4 m_color;
};
