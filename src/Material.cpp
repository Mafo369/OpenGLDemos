#include "Material.h"

Material::Material(Shader* shader){
    m_shader = shader;
}

Material::~Material(){
    delete m_shader;
}
