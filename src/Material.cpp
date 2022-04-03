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

Material::Material(Shader* shader, MaterialParams materialParams, Texture* texture){
    m_shader = shader;
    m_materialParams = materialParams;
    m_hasParams = true;
    m_texture = texture;
    m_hasTexture = true;
}

Material::~Material(){
    delete m_texture;
    delete m_shader;
}
