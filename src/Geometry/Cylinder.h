#pragma once

#include "Mesh.h"

// constants //////////////////////////////////////////////////////////////////
const int MIN_SECTOR_COUNT = 3;
const int MIN_STACK_COUNT  = 1;

class Cylinder : public Mesh {
public:
    Cylinder(glm::vec4 color, float baseRadius, float topRadius, float height, int sectors=MIN_SECTOR_COUNT, int stacks=MIN_STACK_COUNT);
    ~Cylinder();

    std::vector<float> getSideNormals();
    void buildUnitCircleVertices();
    unsigned int baseIndex, topIndex;

private:
    glm::vec4 m_color;
    std::vector<float> unitCircleVertices;

    // memeber vars
    float m_baseRadius;
    float m_topRadius;
    float m_height;
    int m_sectorCount;                        // # of slices
    int m_stackCount;                         // # of stacks
};
