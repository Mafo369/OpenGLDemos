#ifndef SIMPLETRIANGLE_H
#define SIMPLETRIANGLE_H

#include "Buffer.h"
#include "VertexArray.h"
#include "opengldemo.h"

/** Simple drawing demonstration
 */
class SimpleTriangle : public OpenGLDemo {
public:
    explicit SimpleTriangle(int width, int height);
    ~SimpleTriangle() override;

    void draw() override;

    bool keyboard(unsigned char k) override;
private:
    // A simple geometry
    std::vector<GLfloat> _vertices;
    std::vector<GLfloat> _normals;
    std::vector<GLuint> _indices;

    // OpenGL object for geometry
    // Vertex Array Buffer
    VertexArray* m_vao;
    VertexBufferLayout* m_layout = new VertexBufferLayout();
    // Vertex Buffer Object
    VertexBuffer* m_vbo;
    // Normal buffer
    VertexBuffer* m_nbo;
    // Face buffer
    IndexBuffer* m_ebo;

    // Shader program for rendering
    GLuint _program;

    // Different availableprograms
    GLuint _programcolor;
    GLuint _programnormal;

};

#endif // SIMPLETRIANGLE_H
