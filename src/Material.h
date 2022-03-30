#ifndef MATERIAL_H
#define MATERIAL_H
#pragma once

class Shader;

class Material {
public:
    Material(Shader* shader);
    ~Material();

    Shader* getShader() { return m_shader; }
private:
    Shader* m_shader;
};

#endif
