#include "Light.h"
#include "Cube.h"
#include "ext/matrix_transform.hpp"

Light::Light(LightParams lightParams) : RenderObject(){
    m_lightParams = lightParams;
    Shader* programLight = 
        new Shader("/home/mafo/dev/helloOpenGL/Shaders/Camera.vert.glsl", "/home/mafo/dev/helloOpenGL/Shaders/Light.frag.glsl");
    Material* lightMaterial = new Material(programLight);
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
