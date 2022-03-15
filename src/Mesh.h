#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "VertexArray.h"
#include "Buffer.h"
#include "Shader.h"

class Renderer;

struct Vertex{
    glm::vec3 m_position;
    glm::vec3 m_normal;
    glm::vec2 m_texCoords;
};

struct Texture{
    unsigned int m_rendererId;
    std::string m_type;
};

class Mesh{

    friend Renderer;
public:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Texture> m_textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, unsigned int mode);
    ~Mesh();

    void setVertices(std::vector<Vertex> vertices) { m_vertices = vertices; }
    void setIndices(std::vector<unsigned int> indices) { m_indices = indices; }
    void setTextures(std::vector<Texture> textures) { m_textures = textures; }
private:
    VertexArray* m_vao;
    VertexBuffer* m_vbo;
    VertexBuffer* m_nbo;
    IndexBuffer* m_ebo;

    void setupMesh(unsigned int mode);
};
