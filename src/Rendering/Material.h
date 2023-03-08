#ifndef MATERIAL_H
#define MATERIAL_H
#pragma once

#include <glm/glm.hpp>
#include "Texture.h"

class Shader;

typedef struct s_MaterialParams{
    int texDiffuse;
    int texSpecular;
    int texNormal;
    int texAO;
    int texEmission;
    float metallic;
    float roughness;
}MaterialParams;

class Material {
public:
    Material(Shader* shader);
    Material(Shader* shader, MaterialParams materialParams);
    Material(Shader* shader, MaterialParams materialParams, Texture* texture,
             Texture* textureSpecular);
    Material(Shader* shader, MaterialParams materialParams, Texture* texture,
             Texture* textureSpecular, Texture* textureNormal, Texture* textureAO, Texture* textureEmission);
    ~Material();

    Shader* getShader() { return m_shader; }
    MaterialParams& getParams() { return m_materialParams; }
    Texture* getTexture() { return m_texture; }
    Texture* getTextureSpecular() { return m_textureSpecular; }
    bool hasParams() { return m_hasParams; }
    bool hasTexture() { return m_hasTexture; }
    float* getMetallic() { return &m_materialParams.metallic; }
    float* getRoughness() { return &m_materialParams.roughness; }

    Texture* m_texture;
    Texture* m_textureSpecular;
    Texture* m_textureNormal;
    Texture* m_textureAO;
    Texture* m_textureEmission;
private:
    Shader* m_shader;
    MaterialParams m_materialParams;
    bool m_hasParams = false;
    bool m_hasTexture = false;
};

#endif
