# include "hellotriangles.h"

SimpleTriangle::SimpleTriangle(int width, int height) : OpenGLDemo(width, height) {
    // Initialise geometric data
    Vertex v0 = {glm::vec3(0.5f,  0.5f, 0.0f), glm::vec3(0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v1 = {glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v2 = {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v3 = {glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    std::vector<Vertex> vertices = { v0, v1, v2, v3 };
    std::vector<unsigned int> indices = {
        0, 1, 3,   // First Triangle
        1, 2, 3    // Second Triangle
    };
    std::vector<Texture> textures;
 
    m_renderer = new Renderer();

    m_mesh = new Mesh(vertices, indices, textures, GL_TRIANGLES);

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


