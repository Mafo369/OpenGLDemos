#include "RenderObject.h"

RenderObject::RenderObject(){

}

RenderObject::RenderObject(Mesh* mesh, Material* material) {
    m_mesh = mesh;
    m_material = material;
}

RenderObject::~RenderObject(){
    delete m_mesh;
}

