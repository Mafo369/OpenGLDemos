#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H
#pragma once
#include "Material.h"
#include "../Geometry/Mesh.h"

class RenderObject {
public:
    RenderObject();
    RenderObject(Mesh* mesh, Material* material);
    ~RenderObject();

    void setMaterial(Material* material) { m_material = material; }
    Mesh* getMesh() { return m_mesh; }
    Material* getMaterial() { return m_material; }
protected:
    Mesh* m_mesh;
    Material* m_material;
private:
};

#endif
