#pragma once

#include "Mesh.h"
#include "opengldemo.h"
#include "Shader.h"
#include "Buffer.h"
#include "VertexArray.h"

class Renderer
{
public:
    void draw(VertexArray* vao, IndexBuffer* ebo, Shader* shader) const;
    void draw(Mesh* mesh, Shader* shader);
};
