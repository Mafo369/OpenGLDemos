#include "Renderer.h"

void Renderer::draw(VertexArray *vao, IndexBuffer *ebo, Shader* shader) const{
    shader->bind();
    vao->bind();
    ebo->bind();
    glAssert(glDrawElements(ebo->getMode(), ebo->getCount(), GL_UNSIGNED_INT, nullptr));
    vao->unbind();
}

void Renderer::draw(Mesh* mesh, Shader* shader){
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for(unsigned int i = 0; i < mesh->m_textures.size(); i++){
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string name = mesh->m_textures[i].m_type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);

        shader->setUniform1f(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, mesh->m_textures[i].m_rendererId);
    }
    glActiveTexture(GL_TEXTURE0);
    draw(mesh->m_vao, mesh->m_ebo, shader);
}
