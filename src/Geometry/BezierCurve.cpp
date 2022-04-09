#include "BezierCurve.h"
#include "Utils.h"
#include "../Rendering/Renderer.h"

BezierCurve::BezierCurve(std::vector<glm::vec3> controlPoints, unsigned int nbSegments,
                         glm::vec4 color) : Mesh() {
    m_controlPoints = controlPoints;
    m_nbSegments = nbSegments;
    pascalCoeffs(controlPoints.size(), m_coeffsCols);
    computeVertices(color);
    setupMesh(GL_LINES);
}

BezierCurve::~BezierCurve(){

}

void BezierCurve::computeVertices(glm::vec4 color){
    int indice = 0;
    auto normal = glm::vec3(0.577350269189626f, 0.0, 0.0);
    //int n = 100; // Nb de segments en u
    int uf = m_nbSegments-1;
    for(int j = 0; j <= uf; j++){
        float u = float(j)/uf;
        glm::vec3 pu = bezierValue(m_controlPoints, u, m_coeffsCols);
        m_indices.push_back(indice);
        indice++;
        m_indices.push_back(indice);
        Vertex v = { pu, normal, glm::vec2(0.f, 0.f), color };
        m_vertices.push_back(v);
    }
    m_indices.pop_back();
}
