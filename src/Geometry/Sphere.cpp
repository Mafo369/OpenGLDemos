#include "Sphere.h"
#include <GL/gl.h>

Sphere::Sphere(glm::vec4 color) : Mesh() {
    m_color = color;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359f;
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
    {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            m_vertices.push_back({glm::vec3(xPos, yPos, zPos), glm::vec3(xPos, yPos, zPos), glm::vec2(xSegment, ySegment), m_color});
        }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                m_indices.push_back(y * (X_SEGMENTS + 1) + x);
                m_indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                m_indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                m_indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }


    setupMesh(GL_TRIANGLE_STRIP);
}

Sphere::~Sphere(){

}
