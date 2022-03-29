#include "LightCube.h"
#include <GL/gl.h>

LightCube::LightCube(glm::vec3 lightPosition) : Mesh() {
    m_lightPosition = lightPosition;
    m_color = glm::vec4(1.0f);
    m_vertices.push_back({glm::vec3(0.5, 0, 0.5 ), glm::vec3(0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color});
    m_vertices.push_back({glm::vec3(0.5, 0, -0.5), glm::vec3(0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color});
    m_vertices.push_back({glm::vec3(-0.5, 0, -0.5), glm::vec3(-0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color});
    m_vertices.push_back({glm::vec3(-0.5, 0, 0.5), glm::vec3(-0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color});
    m_vertices.push_back({glm::vec3(0.5, 1, 0.5 ), glm::vec3(0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color});
    m_vertices.push_back({glm::vec3(0.5, 1, -0.5), glm::vec3(0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color});
    m_vertices.push_back({glm::vec3(-0.5, 1, -0.5), glm::vec3(-0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color});
    m_vertices.push_back({glm::vec3(-0.5, 1, 0.5), glm::vec3(-0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color});

    m_indices = { 3, 0, 4, 3, 7, 4,
                  0, 1, 5, 0, 4, 5,
                  1, 5, 2, 2, 5, 6,
                  2, 3, 6, 3, 6, 7,
                  0, 1, 3, 2, 1, 3,
                  4, 5, 7, 5, 6, 7};

    setupMesh(GL_TRIANGLES);
}

LightCube::~LightCube(){

}
