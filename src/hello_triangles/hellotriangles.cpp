# include "hellotriangles.h"
#include <iostream>

SimpleTriangle::SimpleTriangle(int width, int height) : OpenGLDemo(width, height) {
    // Initialise geometric data
    _vertices = {
        0.5f,  0.5f, 0.0f,  // Top Right
        0.5f, -0.5f, 0.0f,  // Bottom Right
       -0.5f, -0.5f, 0.0f,  // Bottom Left
       -0.5f,  0.5f, 0.0f   // Top Left
    };
    _normals = {
        0.577350269189626f, 0.577350269189626f, 0.577350269189626f,
        0.577350269189626f, -0.577350269189626f, 0.577350269189626f,
        -0.577350269189626f, -0.577350269189626f, 0.577350269189626f,
        -0.577350269189626f, 0.577350269189626f, 0.577350269189626f
    };
    _indices = {
        0, 1, 3,   // First Triangle
        1, 2, 3    // Second Triangle
    };
 
    // Initialize the geometry
    // 1. Generate geometry buffers
    //glGenVertexArrays(1, &_vao) ;
    m_vao = new VertexArray();
    m_vao->bind();

    m_vbo = new VertexBuffer(_vertices.data(), _vertices.size()*sizeof(GLfloat));
    m_layout->push<float>(3);
    m_vao->addBuffer(m_vbo, m_layout);

    m_nbo = new VertexBuffer(_normals.data(), _normals.size()*sizeof(GLfloat));
    m_vao->addBuffer(m_nbo, m_layout);
 
    m_ebo = new IndexBuffer(_indices.data(), _indices.size());

    //6. Unbind the VAO
    m_vao->unbind();

    m_programcolor = m_program = 
        new Shader("/home/mafo/dev/helloOpenGL/Shaders/Basic.vert.glsl", "/home/mafo/dev/helloOpenGL/Shaders/Basic.frag.glsl");     
    m_programnormal = 
        new Shader("/home/mafo/dev/helloOpenGL/Shaders/Basic.vert.glsl", "/home/mafo/dev/helloOpenGL/Shaders/Normals.frag.glsl");
}

SimpleTriangle::~SimpleTriangle() {
    delete m_programcolor;
    delete m_programnormal;
    delete m_vbo;
    delete m_nbo;
    delete m_ebo;
    delete m_layout;
    delete m_vao;
}

void SimpleTriangle::draw() {
    OpenGLDemo::draw();

    m_program->bind();
    m_vao->bind();
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
    m_vao->unbind();
}

bool SimpleTriangle::keyboard(unsigned char k) {
    switch(k) {
        case 'c' :
            m_program = m_programcolor;
            return true;
        case 'n' :
            m_program = m_programnormal;
            return true;
        default:
            return false;
    }
}


