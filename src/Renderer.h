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
    void setShader(Shader* shader);
    void setMVP(glm::mat4 model, glm::mat4 view, glm::mat4 projection);
    void clearRenderObjects() { m_roList.clear(); }
    Shader* getCurrentShader() { return m_roList[0]->getShader(); }


    void draw(VertexArray* vao, IndexBuffer* ebo, Shader* shader) const;
    void draw(Mesh* mesh, Shader* shader);
    void draw();

private:
    std::vector<RenderObject*> m_roList;
};
