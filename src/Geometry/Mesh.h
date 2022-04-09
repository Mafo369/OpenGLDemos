#ifndef MESH_H
#define MESH_H
#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "../Rendering/VertexArray.h"
#include "../Rendering/Buffer.h"
#include "../Rendering/Shader.h"
#include "../Rendering/Texture.h"

class Renderer;

struct Vertex{
    glm::vec3 m_position;
    glm::vec3 m_normal;
    glm::vec2 m_texCoords;
    glm::vec4 m_color;
};

class Mesh{

    friend Renderer;
public:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;

    Mesh() {}
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int mode);
    ~Mesh();

    void setVertices(std::vector<Vertex> vertices) { m_vertices = vertices; }
    void setIndices(std::vector<unsigned int> indices) { m_indices = indices; }
private:
    VertexArray* m_vao;
    VertexBuffer* m_vbo;
    IndexBuffer* m_ebo;

protected:
    void setupMesh(unsigned int mode);
};

#endif
