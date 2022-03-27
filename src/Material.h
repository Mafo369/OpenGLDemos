#pragma once

#include "Shader.h"

class Material {
public:
    Material(Shader* shader);
    ~Material();

    Shader* getShader() { return m_shader; }
private:
    Shader* m_shader;
};
