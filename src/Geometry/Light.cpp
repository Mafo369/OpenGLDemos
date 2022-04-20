#include "Light.h"
#include "Cube.h"
#include "ext/matrix_transform.hpp"
#include <memory>

Light::Light(LightParams lightParams) : RenderObject(){
    m_lightParams = lightParams;
    Shader* programLight = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/Light.frag.glsl");
    std::shared_ptr<Material> lightMaterial = std::make_shared<Material>(programLight);
    m_mesh = new Cube(lightParams.position);
    m_material = lightMaterial;
}

Light::~Light(){

}

void Light::update(glm::vec3 translation){
    m_lightmodel = glm::translate(glm::mat4(1.0), translation);
    m_lightmodel = glm::translate(m_lightmodel, m_lightParams.position);
    m_lightmodel = glm::scale(m_lightmodel, glm::vec3(0.2f));
}
