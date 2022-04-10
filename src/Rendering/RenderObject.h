#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H
#include <memory>
#pragma once
#include "Material.h"
#include "../Geometry/Mesh.h"

class RenderObject {
public:
    RenderObject();
    RenderObject(Mesh* mesh, std::shared_ptr<Material> material);
    ~RenderObject();

    void setMaterial(std::shared_ptr<Material> material) { m_material = material; }
    Mesh* getMesh() { return m_mesh; }
    std::shared_ptr<Material> getMaterial() { return m_material; }
protected:
    Mesh* m_mesh;
    std::shared_ptr<Material> m_material;
private:
};

#endif
