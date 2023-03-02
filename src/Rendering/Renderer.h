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
    void setMaterial(std::shared_ptr<Material> material);
    void setMaterialParams();
    void setLightMVP(glm::mat4 model, glm::mat4 view, glm::mat4 projection, unsigned int id);
    void setVP(glm::mat4 view, glm::mat4 projection);
    void setLight(Light* light, unsigned int id);
    void setCameraPosition(glm::vec3 position);
    void clearRenderObjects(); 
    std::shared_ptr<Material> getCurrentMaterial() { if(!m_roList.empty()) return m_roList[0]->getMaterial(); else return nullptr; }


    void setEnvMap(const std::string& path);
    bool hasCubeMap() { return m_hasCubeMap; }
    unsigned int getCubeMap() { return m_envCubeMap; }

    void draw(VertexArray* vao, IndexBuffer* ebo, Shader* shader) const;
    void draw(Mesh* mesh, Shader* shader);
    void draw(Shader* shader);
    void draw();

private:
    std::vector<RenderObject*> m_roList;
    std::vector<RenderObject*> m_roLights;
    unsigned int m_envMap;
    unsigned int m_envCubeMap;

    Shader* m_programBg;
    Mesh* m_cubeMesh;

    bool m_hasCubeMap;

    glm::mat4 m_view, m_projection;
};

#endif
