#include "Mesh.h"

class BezierSurface : public Mesh{
public:
    BezierSurface(std::vector<std::vector<glm::vec3>> controlPoints, unsigned int nbSegmentsU,
                  unsigned int nbSegmentsV, glm::vec4 color);
    ~BezierSurface();

private:
    void computeVertices(unsigned int lignes, glm::vec4 color); // Indices will be computed along
    void computeIndices();
    void computeNormals();

    std::vector<std::vector<glm::vec3>> m_controlPoints;
    std::vector<std::vector<int>> m_coeffsCols;
    std::vector<std::vector<int>> m_coeffsLignes;
    std::vector<std::vector<unsigned int>> m_vertexNormals;
    unsigned int m_nbSegmentsU;
    unsigned int m_nbSegmentsV;
};
