#include "Material.h"
#include "Shader.h"

Material::Material(Shader* shader){
    m_shader = shader;
}

Material::Material(Shader* shader, MaterialParams materialParams){
    m_shader = shader;
    m_materialParams = materialParams;
    m_hasParams = true;
}

Material::Material(Shader* shader, MaterialParams materialParams, Texture* texture,
                   Texture* textureSpecular){
    m_shader = shader;
    m_materialParams = materialParams;
    m_hasParams = true;
    m_texture = texture;
    m_textureSpecular = textureSpecular;
    m_hasTexture = true;
}

Material::Material(Shader* shader, MaterialParams materialParams, Texture* texture,
                   Texture* textureSpecular, Texture* textureNormal, Texture* textureAO, Texture* textureEmission){
    m_shader = shader;
    m_materialParams = materialParams;
    m_hasParams = true;
    m_texture = texture;
    m_textureSpecular = textureSpecular;
    m_textureNormal = textureNormal;
    m_textureEmission = textureEmission;
    m_textureAO = textureAO;
    m_hasTexture = true;
}

Material::~Material(){
    delete m_shader;
    if(m_hasTexture){
        delete m_texture;
        delete m_textureSpecular;
    }
}
