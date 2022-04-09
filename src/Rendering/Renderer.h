#ifndef RENDERER_H
#define RENDERER_H
#pragma once

#include "Geometry/Mesh.h"
#include "RenderObject.h"
#include "../demos/opengldemo.h"
#include "Shader.h"
#include "Buffer.h"
#include "VertexArray.h"

class Renderer
{
public:
    Renderer();
    ~Renderer();
    void addRenderObject(RenderObject* ro);
    void addLightRo(RenderObject* ro);
    void setMaterial(Material* material);
    void setMaterialParams();
    void setLightMVP(glm::mat4 model, glm::mat4 view, glm::mat4 projection, unsigned int id);
    void setMVP(glm::mat4 model, glm::mat4 view, glm::mat4 projection);
    void setLight(Light* light, unsigned int id);
    void setCameraPosition(glm::vec3 position);
    void clearRenderObjects(); 
    Material* getCurrentMaterial() { return m_roList[0]->getMaterial(); }


    void draw(VertexArray* vao, IndexBuffer* ebo, Shader* shader) const;
    void draw(Mesh* mesh, Shader* shader);
    void draw();

private:
    std::vector<RenderObject*> m_roList;
    std::vector<RenderObject*> m_roLights;
};

#endif
