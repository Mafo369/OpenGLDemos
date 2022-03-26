#include "Mesh.h"

class BezierCurve : public Mesh{
public:
    BezierCurve(std::vector<glm::vec3> controlPoints, unsigned int nbSegments);
    ~BezierCurve();

private:
    void computeVertices(); // Indices will be computed along

    std::vector<glm::vec3> m_controlPoints;
    std::vector<std::vector<int>> m_coeffsCols;
    unsigned int m_nbSegments;
};
