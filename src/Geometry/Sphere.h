#pragma once

#include "Mesh.h"

class Sphere : public Mesh {
public:
    Sphere(glm::vec4 color);
    ~Sphere();

private:
    glm::vec4 m_color;
};
