#ifndef SIMPLETRIANGLE_H
#define SIMPLETRIANGLE_H

#include "Renderer.h"
#include "imgui.h"

/** Simple drawing demonstration
 */
class SimpleTriangle : public OpenGLDemo {
public:
    explicit SimpleTriangle(int width, int height, ImVec4 clearColor);
    ~SimpleTriangle() override;

    void draw() override;

    bool keyboard(unsigned char k) override;

    void compute() override {}
    std::vector<std::vector<glm::vec3>>& getControlsPoints() override { return m_controlPoints; }
    glm::vec4& getColor() override { return m_color; }
private:
    // A simple geometry
    Mesh* m_mesh;
    std::vector<std::vector<glm::vec3>> m_controlPoints;

    glm::vec4 m_color;
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
