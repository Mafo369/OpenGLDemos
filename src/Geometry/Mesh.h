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
class Bone;

struct Vertex{
    glm::vec3 m_position;
    glm::vec3 m_normal;
    glm::vec2 m_texCoords;
    glm::vec4 m_color;

    glm::vec3 m_tangent;
    glm::vec3 m_bitTangent;
};

class Mesh{

    friend Renderer;
public:
    std::vector<Vertex> m_vertices;
    std::vector<Vertex> m_verticesInit;
    std::vector<unsigned int> m_indices;

    Mesh() {}
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int mode);
    ~Mesh();

    void setVertices(std::vector<Vertex> vertices) { m_vertices = vertices; }
    void setIndices(std::vector<unsigned int> indices) { m_indices = indices; }
    void setColor(const glm::vec4& color);

    std::vector<Bone*> m_boneList;
    std::vector<std::vector<std::pair<int,float>>> m_boneWeight;
private:
    VertexArray* m_vao;
    VertexBuffer* m_vbo;
    IndexBuffer* m_ebo;

protected:
    void setupMesh(unsigned int mode);
};

#endif
