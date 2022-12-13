#include "RenderObject.h"

RenderObject::RenderObject() {}

RenderObject::RenderObject( Mesh* mesh, std::shared_ptr<Material> material ) {
    m_mesh     = mesh;
    m_material = material;
}

RenderObject::~RenderObject() {
    delete m_mesh;
}
