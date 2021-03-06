# include "hellotriangles.h"

SimpleTriangle::SimpleTriangle(int width, int height, ImVec4 clearColor) : OpenGLDemo(width, height, clearColor) {
    // Initialise geometric data
    m_color = glm::vec4(1.0f, 0.5f, 0.2f, 1.f);
    Vertex v0 = {normalize(glm::vec3(0.5f,  0.5f, 1.5f)), glm::vec3(0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
    Vertex v1 = {normalize(glm::vec3(0.5f, -0.5f, 0.0f)), glm::vec3(0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
    Vertex v2 = {normalize(glm::vec3(-0.5f, -0.5f, 0.0f)), glm::vec3(0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
    Vertex v3 = {normalize(glm::vec3(-0.5f, 0.5f, 1.5f)), glm::vec3(0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
    std::vector<Vertex> vertices = { v0, v1, v2, v3 };
    std::vector<unsigned int> indices = {
        0, 1, 3,   // First Triangle
        1, 2, 3    // Second Triangle
    };
 
    m_renderer = new Renderer();

    m_mesh = new Mesh(vertices, indices, GL_TRIANGLES);

    m_programcolor = m_program = 
        new Shader("/home/mafo/dev/helloOpenGL/Shaders/Basic.vert.glsl", "/home/mafo/dev/helloOpenGL/Shaders/Basic.frag.glsl");     
    m_programnormal = 
        new Shader("/home/mafo/dev/helloOpenGL/Shaders/Basic.vert.glsl", "/home/mafo/dev/helloOpenGL/Shaders/Normals.frag.glsl");
}

SimpleTriangle::~SimpleTriangle() {
    delete m_programcolor;
    delete m_programnormal;
    delete m_mesh;
    delete m_renderer;
}

void SimpleTriangle::draw() {
    OpenGLDemo::draw();

    m_renderer->draw(m_mesh, m_program);
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


