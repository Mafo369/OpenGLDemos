#include "Material.h"
#include "Shader.h"

Material::Material(Shader* shader){
    m_shader = shader;
}

Material::~Material(){
    delete m_shader;
}
