#include "Plane.h"
#include <GL/gl.h>

Plane::Plane(glm::vec4 color) : Mesh() {
    m_color = color;

    m_vertices.push_back({glm::vec3(-1., 0,  1.), glm::vec3(0,1,0), glm::vec2(0.f, 1.f), m_color});
    m_vertices.push_back({glm::vec3(-1., 0, -1.), glm::vec3(0,1,0), glm::vec2(0.f, 0.f), m_color});
    m_vertices.push_back({glm::vec3(1., 0, -1.), glm::vec3(0,1,0), glm::vec2(1.f, 0.f), m_color});
    m_vertices.push_back({glm::vec3(-1., 0, 1.), glm::vec3(0,1,0), glm::vec2(0.f, 1.f), m_color});
    m_vertices.push_back({glm::vec3(1., 0, -1.), glm::vec3(0,1,0), glm::vec2(1.f, 0.f), m_color});
    m_vertices.push_back({glm::vec3(1., 0, 1.), glm::vec3(0,1,0), glm::vec2(1.f, 1.f), m_color});

    m_indices = {
        0, 1, 2,   // First Triangle
        3, 4, 5    // Second Triangle
    };

    setupMesh(GL_TRIANGLES);
}

Plane::~Plane(){

}
