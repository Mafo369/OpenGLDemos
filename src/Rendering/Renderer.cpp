#include "Renderer.h"

Renderer::Renderer(){

}

Renderer::~Renderer(){

}

void Renderer::draw(VertexArray *vao, IndexBuffer *ebo, Shader* shader) const{
    shader->bind();
    vao->bind();
    ebo->bind();
    glAssert(glDrawElements(ebo->getMode(), ebo->getCount(), GL_UNSIGNED_INT, nullptr));
    vao->unbind();
}

void Renderer::draw(Mesh* mesh, Shader* shader){
    //unsigned int diffuseNr = 1;
    //unsigned int specularNr = 1;
    //for(unsigned int i = 0; i < mesh->m_textures.size(); i++){
    //    glActiveTexture(GL_TEXTURE0 + i);
    //    std::string number;
    //    std::string name = mesh->m_textures[i].m_type;
    //    if(name == "texture_diffuse")
    //        number = std::to_string(diffuseNr++);
    //    else if(name == "texture_specular")
    //        number = std::to_string(specularNr++);

    //    shader->setUniform1f(("material." + name + number).c_str(), i);
    //    glBindTexture(GL_TEXTURE_2D, mesh->m_textures[i].m_rendererId);
    //}
    //glActiveTexture(GL_TEXTURE0);
    draw(mesh->m_vao, mesh->m_ebo, shader);
}

void Renderer::draw() {
    for(auto& ro : m_roList) {
        Mesh* mesh = ro->getMesh();
        draw(mesh->m_vao, mesh->m_ebo, ro->getMaterial()->getShader());
    }
    for(auto& ro : m_roLights) {
        Mesh* mesh = ro->getMesh();
        draw(mesh->m_vao, mesh->m_ebo, ro->getMaterial()->getShader());
    }
}

void Renderer::addRenderObject(RenderObject* ro){
    m_roList.push_back(ro);
}

void Renderer::addLightRo(RenderObject* ro){
    m_roLights.push_back(ro);
}

void Renderer::setMaterial(Material* material){
    for(auto& ro: m_roList){
        ro->setMaterial(material);
    }
}

void Renderer::setMaterialParams(){
    for(auto& ro : m_roList){
        auto material = ro->getMaterial();
        if(material->hasParams()){
            auto materialParams = material->getParams();
            if(material->hasTexture()){
                auto texture = material->getTexture();
                auto textureSpecular = material->getTextureSpecular();
                texture->bind();
                textureSpecular->bind(1);
                material->getShader()->setMaterialParams(materialParams);
            }
            else
                material->getShader()->setMaterialParams(materialParams);
        }

    }
}

void Renderer::setLightMVP(glm::mat4 model, glm::mat4 view, glm::mat4 projection, unsigned int id){
    m_roLights[id]->getMaterial()->getShader()->setMVP(model, view, projection);
}

void Renderer::setMVP(glm::mat4 model, glm::mat4 view, glm::mat4 projection){
    for(auto& ro: m_roList){
        ro->getMaterial()->getShader()->setMVP(model, view, projection);
    }
    for(auto& ro: m_roLights){
        ro->getMaterial()->getShader()->setMVP(model, view, projection);
    }
}

void Renderer::setLight(Light* light, unsigned int id){
    for(auto& ro: m_roList){
        ro->getMaterial()->getShader()->setLight(light, id);
    }
}

void Renderer::setCameraPosition(glm::vec3 position){
    for(auto& ro: m_roList){
        ro->getMaterial()->getShader()->setCameraPosition(position);
    }
}
