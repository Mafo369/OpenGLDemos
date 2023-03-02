#include "Renderer.h"
#include "Framebuffer.h"
#include "../Geometry/Cube.h"
#include "../../../libs/stb_image.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

Renderer::Renderer(){
  m_hasCubeMap = false;
}

Renderer::~Renderer(){

}

void Renderer::setEnvMap(const std::string& path) {
    stbi_set_flip_vertically_on_load(true);
    int cmWidth, cmHeight, nrComponents;
    float *data = stbi_loadf(path.c_str(), &cmWidth, &cmHeight, &nrComponents, 0);
    if (data)
    {
        glGenTextures(1, &m_envMap);
        glBindTexture(GL_TEXTURE_2D, m_envMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, cmWidth, cmHeight, 0, GL_RGB, GL_FLOAT, data); 

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load environnement map." << std::endl;
        return ;
    }  

    m_cubeMesh = new Cube({0.1,0.1,0.8,1});

    Shader* programCube =
        new Shader("Shaders/Cubemap.vert.glsl", "Shaders/Cubemap.frag.glsl");
    m_programBg =
        new Shader("Shaders/Background.vert.glsl", "Shaders/Background.frag.glsl");

    programCube->bind();
    programCube->setUniform1i("equirectangularMap", 0);
    programCube->unbind();

    m_programBg->bind();
    m_programBg->setUniform1i("environmentMap", 0);
    m_programBg->unbind();

    Framebuffer* captureFbo = new Framebuffer();
    unsigned int captureRBO;
    glGenRenderbuffers(1, &captureRBO);

    captureFbo->bind();

    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cmWidth, cmWidth);
    captureFbo->attachRenderbuffer(GL_DEPTH_ATTACHMENT, captureRBO);

    glGenTextures(1, &m_envCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubeMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 
                     cmWidth, cmWidth, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = 
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // convert HDR equirectangular environment map to cubemap equivalent
    programCube->bind();
    programCube->setUniform1i("equirectangularMap", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_envMap);

    glViewport(0, 0, cmWidth, cmWidth); // don't forget to configure the viewport to the capture dimensions.
    captureFbo->bind();
    for (unsigned int i = 0; i < 6; ++i)
    {
        programCube->setMVP(glm::mat4(1.f), captureViews[i], captureProjection);
        captureFbo->attachCubemapTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_envCubeMap);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw(m_cubeMesh, programCube);
    }
    captureFbo->unbind();

    m_hasCubeMap = true;
}

void Renderer::draw(VertexArray *vao, IndexBuffer *ebo, Shader* shader) const{
    shader->bind();
    vao->bind();
    ebo->bind();
    glAssert(glDrawElements(ebo->getMode(), ebo->getCount(), GL_UNSIGNED_INT, nullptr));
    vao->unbind();
}

void Renderer::draw(Mesh* mesh, Shader* shader){
    draw(mesh->m_vao, mesh->m_ebo, shader);
}

void Renderer::draw() {
    for(auto& ro : m_roList) {
        Mesh* mesh = ro->getMesh();
        ro->getMaterial()->getShader()->setMVP(ro->getTransform(), m_view, m_projection);
        draw(mesh->m_vao, mesh->m_ebo, ro->getMaterial()->getShader());
    }
    for(auto& ro : m_roLights) {
        Mesh* mesh = ro->getMesh();
        draw(mesh->m_vao, mesh->m_ebo, ro->getMaterial()->getShader());
    }

    if(m_hasCubeMap){
      m_programBg->bind();
      m_programBg->setMVP(glm::mat4(1.f), m_view, m_projection);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubeMap);
      draw(m_cubeMesh, m_programBg);
    }
}

void Renderer::draw(Shader* shader) {
    for(auto& ro : m_roList) {
        Mesh* mesh = ro->getMesh();
        shader->setMVP(ro->getTransform(), m_view, m_projection);
        draw(mesh->m_vao, mesh->m_ebo, shader);
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

void Renderer::setMaterial(std::shared_ptr<Material> material){
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

void Renderer::setVP(glm::mat4 view, glm::mat4 projection){
    m_view = view;
    m_projection = projection;
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

void Renderer::clearRenderObjects(){
    for(auto& ro : m_roList){
        delete ro;
    }
    m_roList.clear();
}
