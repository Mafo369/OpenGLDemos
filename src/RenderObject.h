#pragma once
#include "Shader.h"
#include "Geometry/Mesh.h"

class RenderObject {
public:
    RenderObject(Mesh* mesh, Shader* shader);
    ~RenderObject();

    void setShader(Shader* shader) { m_shader = shader; }
    Mesh* getMesh() { return m_mesh; }
    Shader* getShader() { return m_shader; }
private:
    Mesh* m_mesh;
    Shader* m_shader;
};
