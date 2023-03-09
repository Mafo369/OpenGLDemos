#include "Mesh.h"
#include "../Rendering/Renderer.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, 
           unsigned int mode){
    setVertices(vertices);
    setIndices(indices);
    setupMesh(mode);
}

Mesh::~Mesh(){
    delete m_vao;
    delete m_ebo;
    delete m_vbo;
}

void Mesh::setColor(const glm::vec4& color){
  for(auto& v : m_vertices){
    v.m_color = color;
  }
  setupMesh(GL_TRIANGLES);
}

void Mesh::setupMesh(unsigned int mode){
    m_vao = new VertexArray();
    m_vao->bind();

    m_vbo = new VertexBuffer(m_vertices.data(), m_vertices.size()*sizeof(Vertex));

    auto layout = VertexBufferLayout();
    layout.push<float>(3);
    layout.push<float>(3);
    layout.push<float>(2);
    layout.push<float>(4);
    layout.push<float>(3);
    layout.push<float>(3);
    m_vao->addBuffer(m_vbo, &layout);
 
    m_ebo = new IndexBuffer(m_indices.data(), m_indices.size(), mode);

    m_vao->unbind();
}
