#include "hellotriangles.h"
#include "Buffer.h"
#include <iostream>

static const char* vertexshader_source ="#version 410 core\n\
        layout (location = 0) in vec3 position;\n\
        layout (location = 1) in vec3 inormal;\n\
        out vec3 normal;\n\
        void main()\n\
        {\n\
            normal=inormal;\n\
            gl_Position = vec4(position.x, position.y, position.z, 1.0);\n\
        }\n";

static const char* fragmentshader_source ="#version 410 core\n\
        in vec3 normal;\n\
        out vec4 color;\n\
        void main()\n\
        {\n\
            color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n\
        }\n";

static const char* fragmentshadernormal_source ="#version 410 core\n\
        in vec3 normal;\n\
        out vec4 color;\n\
        void main()\n\
        {\n\
            color = vec4(normal*0.5+0.5, 1.0f);\n\
        }\n";

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

    m_vbo = new VertexBuffer(_vertices.data(), _vertices.size()*sizeof (GLfloat));
    m_layout->push<float>(3);
    m_vao->addBuffer(m_vbo, m_layout);

    m_ebo = new IndexBuffer(_indices.data(), _indices.size());

    m_nbo = new VertexBuffer(_normals.data(), _normals.size()*sizeof (GLfloat));
    m_vao->addBuffer(m_nbo, m_layout);

    //6. Unbind the VAO
    m_vao->unbind();

    // Initialize shaders
    GLint success;
    GLchar infoLog[512]; // warning fixed size ... request for LOG_LENGTH!!!
    GLuint vertexshader, fragmentshader;

    // 1. Generate the shader
    vertexshader = glCreateShader(GL_VERTEX_SHADER);
    // 2. set the source
    glShaderSource(vertexshader, 1, &vertexshader_source, NULL);
    // 3. Compile
    glCompileShader(vertexshader);
    // 4. test for compile error
    glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexshader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentshader, 1, &fragmentshader_source, NULL);
    glCompileShader(fragmentshader);
    glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentshader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 1. Generate the program
    _programcolor = _program = glCreateProgram();
    // 2. Attach the shaders to the program
    glAttachShader(_program, vertexshader);
    glAttachShader(_program, fragmentshader);
    // 3. Link the program
    glLinkProgram(_program);
    // 4. Test for link errors
    glGetProgramiv(_program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(_program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::LINK_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(fragmentshader);

    fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentshader, 1, &fragmentshadernormal_source, NULL);
    glCompileShader(fragmentshader);
    glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentshader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 1. Generate the program
    _programnormal = glCreateProgram();
    // 2. Attach the shaders to the program
    glAttachShader(_programnormal, vertexshader);
    glAttachShader(_programnormal, fragmentshader);
    // 3. Link the program
    glLinkProgram(_programnormal);
    // 4. Test for link errors
    glGetProgramiv(_programnormal, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(_programnormal, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::LINK_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(fragmentshader);
    glDeleteShader(vertexshader);
}

SimpleTriangle::~SimpleTriangle() {
    glDeleteProgram(_programcolor);
    glDeleteProgram(_programnormal);
    delete m_vbo;
    delete m_nbo;
    delete m_ebo;
    delete m_vao;
}

void SimpleTriangle::draw() {
    OpenGLDemo::draw();

    glUseProgram(_program);
    m_vao->bind();
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
    m_vao->unbind();
}

bool SimpleTriangle::keyboard(unsigned char k) {
    switch(k) {
        case 'c' :
            _program = _programcolor;
            return true;
        case 'n' :
            _program = _programnormal;
            return true;
        default:
            return false;
    }
}


