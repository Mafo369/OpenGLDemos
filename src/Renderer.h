#pragma once

#include "Geometry/Mesh.h"
#include "RenderObject.h"
#include "opengldemo.h"
#include "Shader.h"
#include "Buffer.h"
#include "VertexArray.h"

class Renderer
{
public:
    void addRenderObject(RenderObject* ro);
    void setMaterial(Material* material);
    void setMVP(glm::mat4 model, glm::mat4 view, glm::mat4 projection);
    void setLight(Light light);
    void clearRenderObjects() { m_roList.clear(); }
    Material* getCurrentMaterial() { return m_roList[0]->getMaterial(); }


    void draw(VertexArray* vao, IndexBuffer* ebo, Shader* shader) const;
    void draw(Mesh* mesh, Shader* shader);
    void draw();

private:
    std::vector<RenderObject*> m_roList;
};
