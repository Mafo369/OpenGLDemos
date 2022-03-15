#ifndef SIMPLETRIANGLE_H
#define SIMPLETRIANGLE_H

#include "Renderer.h"

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
    Mesh* m_mesh;

    // OpenGL object for geometry
    // Vertex Array Buffer
    VertexArray* m_vao;
    // Vertex Buffer Object
    VertexBuffer* m_vbo;
    // Normal buffer
    VertexBuffer* m_nbo;
    // Face buffer
    IndexBuffer* m_ebo;

    // Shader program for rendering
    Shader* m_program;

    // Different availableprograms
    Shader* m_programcolor;
    Shader* m_programnormal;

    Renderer* m_renderer;

};

#endif // SIMPLETRIANGLE_H
