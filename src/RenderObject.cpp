#include "RenderObject.h"

RenderObject::RenderObject(Mesh* mesh, Shader* shader) {
    m_mesh = mesh;
    m_shader = shader;
}

RenderObject::~RenderObject(){
    delete m_mesh;
    delete m_shader;
}

