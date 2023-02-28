#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H
#include <memory>
#pragma once
#include "Material.h"
#include "../Geometry/Mesh.h"
#include "../Geometry/Utils.h"

class RenderObject {
public:
    RenderObject();
    RenderObject(Mesh* mesh, std::shared_ptr<Material> material);
    ~RenderObject();

    void setMaterial(std::shared_ptr<Material> material) { m_material = material; }
    Mesh* getMesh() { return m_mesh; }
    std::shared_ptr<Material> getMaterial() { return m_material; }
    void setTransform(glm::mat4& transform) { m_transform = transform; }
    glm::mat4 getTransform() { return m_transform; }
protected:
    int m_rendererId = -1;
    Mesh* m_mesh;
    std::shared_ptr<Material> m_material;
    glm::mat4 m_transform = glm::mat4(1.f);
private:
};

#endif
