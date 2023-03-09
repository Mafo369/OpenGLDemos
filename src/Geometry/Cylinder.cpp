#include "Cylinder.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>
#include "Rendering/Renderer.h"

Cylinder::Cylinder(glm::vec4 color, float baseRadius, float topRadius, float height, int sectors, int stacks)
    : m_baseRadius(baseRadius), m_topRadius(topRadius), m_height(height), m_sectorCount(sectors), m_stackCount(stacks) {


    glm::vec3 a {0, 0, 0};
    glm::vec3 b {0, height, 0};
    glm::vec3 xPlane {1, 0, 0};
    glm::vec3 yPlane {0, 0, 1};

    int nFaces = 32;
    float thetaInc = (M_PI*2) / nFaces;

    for ( int i = 0; i <= nFaces; i++ )
    {
        float theta = i * thetaInc;
        for(int j=0; j<sectors; j++){
            auto p = glm::vec3(j/(float)sectors) * (a+b);
            float phi = j  * M_PI / (sectors-1);
            Vertex v = {{p + baseRadius * (std::cos( theta ) * xPlane + std::sin( theta ) * yPlane)}, 
                        { glm::vec3(std::cos(theta), 0, std::sin(theta))}, 
                        {0.5 * theta / M_PI, phi / M_PI}, color, {0,0,0}, {0,0,0}};
            m_vertices.push_back(v);
        }
    }

    for ( int i = 0; i < nFaces ; ++i){
        for ( int j = 0; j < sectors-1; ++j )
        {
            unsigned int idx1 = i * sectors + j;
            unsigned int idx2 = (i+1) * sectors + j;
            unsigned int idx3 = idx1 + 1;
            unsigned int idx4 = idx2 + 1;

            m_indices.push_back(idx1);
            m_indices.push_back(idx2);
            m_indices.push_back(idx3);
            m_indices.push_back(idx2);
            m_indices.push_back(idx4);
            m_indices.push_back(idx3);
        }
    }
    setupMesh(GL_TRIANGLES);

    //// generate unit circle vertices first
    //buildUnitCircleVertices();

    //float x, y, z;                                  // vertex position
    ////float s, t;                                     // texCoord
    //float radius;                                   // radius for each stack

    //// get normals for cylinder sides
    //std::vector<float> sideNormals = getSideNormals();

    //// put vertices of side cylinder to array by scaling unit circle
    //for(int i = 0; i <= stacks; ++i)
    //{
    //    z = -(height * 0.5f) + (float)i / stacks * height;      // vertex position z
    //    radius = baseRadius + (float)i / stacks * (topRadius - baseRadius);     // lerp
    //    float t = 1.0f - (float)i / stacks;   // top-to-bottom

    //    for(int j = 0, k = 0; j <= sectors; ++j, k += 3)
    //    {
    //        x = unitCircleVertices[k];
    //        y = unitCircleVertices[k+1];
    //        Vertex v = {{x * radius, y * radius, z}, {sideNormals[k], sideNormals[k+1], sideNormals[k+2]}, {(float)j / sectors, t}, color, {0,0,0}, {0,0,0}};
    //        m_vertices.push_back(v);
    //    }
    //}

    //// remember where the base.top vertices start
    //unsigned int baseVertexIndex = (unsigned int)m_vertices.size();

    //// put vertices of base of cylinder
    //z = -height * 0.5f;
    //Vertex vB = {{0, 0, z}, {0, 0, -1}, {0.5f, 0.5f}, color, {0,0,0}, {0,0,0}};
    //m_vertices.push_back(vB);
    //for(int i = 0, j = 0; i < sectors; ++i, j += 3)
    //{
    //    x = unitCircleVertices[j];
    //    y = unitCircleVertices[j+1];
    //    Vertex v = {{x * baseRadius, y * baseRadius, z}, {0, 0, -1}, {-x * 0.5f + 0.5f, -y * 0.5f + 0.5f}, color, {0,0,0}, {0,0,0}};
    //    m_vertices.push_back(v);
    //}

    //// remember where the base vertices start
    //unsigned int topVertexIndex = (unsigned int)m_vertices.size();

    //// put vertices of top of cylinder
    //z = height * 0.5f;
    //Vertex vT = {{0, 0, z}, {0, 0, 1}, {0.5f, 0.5f}, color, {0,0,0}, {0,0,0}};
    //m_vertices.push_back(vT);
    //for(int i = 0, j = 0; i < sectors; ++i, j += 3)
    //{
    //    x = unitCircleVertices[j];
    //    y = unitCircleVertices[j+1];
    //    Vertex v = {{x * topRadius, y * topRadius, z}, {0, 0, 1}, {x * 0.5f + 0.5f, -y * 0.5f + 0.5f}, color, {0,0,0}, {0,0,0}};
    //    m_vertices.push_back(v);
    //}

    //// put indices for sides
    //unsigned int k1, k2;
    //for(int i = 0; i < stacks; ++i)
    //{
    //    k1 = i * (sectors + 1);     // bebinning of current stack
    //    k2 = k1 + sectors + 1;      // beginning of next stack

    //    for(int j = 0; j < sectors; ++j, ++k1, ++k2)
    //    {
    //        // 2 trianles per sector
    //        m_indices.push_back(k1);
    //        m_indices.push_back(k1 + 1);
    //        m_indices.push_back(k2);
    //        m_indices.push_back(k2);
    //        m_indices.push_back(k1 + 1);
    //        m_indices.push_back(k2 + 1);
    //    }
    //}

    //// remember where the base indices start
    //baseIndex = (unsigned int)m_indices.size();

    //// put indices for base
    //for(int i = 0, k = baseVertexIndex + 1; i < sectors; ++i, ++k)
    //{
    //    if(i < (sectors - 1)){
    //        m_indices.push_back(baseVertexIndex);
    //        m_indices.push_back(k + 1);
    //        m_indices.push_back(k);
    //    }
    //    else {    // last triangle
    //        m_indices.push_back(baseVertexIndex);
    //        m_indices.push_back(baseVertexIndex + 1);
    //        m_indices.push_back(k);
    //    }
    //}

    //// remember where the base indices start
    //topIndex = (unsigned int)m_indices.size();

    //for(int i = 0, k = topVertexIndex + 1; i < sectors; ++i, ++k)
    //{
    //    if(i < (sectors - 1)){
    //        m_indices.push_back(topVertexIndex);
    //        m_indices.push_back(k);
    //        m_indices.push_back(k+1);
    //    }
    //    else{
    //        m_indices.push_back(topVertexIndex);
    //        m_indices.push_back(k);
    //        m_indices.push_back(topVertexIndex + 1);
    //    }
    //}

    //// generate interleaved vertex array as well
    ////buildInterleavedVertices();
    //setupMesh(GL_TRIANGLES);
}

Cylinder::~Cylinder() {

}

///////////////////////////////////////////////////////////////////////////////
// generate shared normal vectors of the side of cylinder
///////////////////////////////////////////////////////////////////////////////
std::vector<float> Cylinder::getSideNormals()
{
    const float PI = acos(-1);
    float sectorStep = 2 * PI / m_sectorCount;
    float sectorAngle;  // radian

    // compute the normal vector at 0 degree first
    // tanA = (baseRadius-topRadius) / height
    float zAngle = atan2(m_baseRadius - m_topRadius, m_height);
    float x0 = cos(zAngle);     // nx
    float y0 = 0;               // ny
    float z0 = sin(zAngle);     // nz

    // rotate (x0,y0,z0) per sector angle
    std::vector<float> normals;
    for(int i = 0; i <= m_sectorCount; ++i)
    {
        sectorAngle = i * sectorStep;
        normals.push_back(cos(sectorAngle)*x0 - sin(sectorAngle)*y0);   // nx
        normals.push_back(sin(sectorAngle)*x0 + cos(sectorAngle)*y0);   // ny
        normals.push_back(z0);  // nz
        /*
        //debug
        float nx = cos(sectorAngle)*x0 - sin(sectorAngle)*y0;
        float ny = sin(sectorAngle)*x0 + cos(sectorAngle)*y0;
        std::cout << "normal=(" << nx << ", " << ny << ", " << z0
                  << "), length=" << sqrtf(nx*nx + ny*ny + z0*z0) << std::endl;
        */
    }

    return normals;
}

///////////////////////////////////////////////////////////////////////////////
// generate 3D vertices of a unit circle on XY plance
///////////////////////////////////////////////////////////////////////////////
void Cylinder::buildUnitCircleVertices()
{
    const float PI = acos(-1);
    float sectorStep = 2 * PI / m_sectorCount;
    float sectorAngle;  // radian

    std::vector<float>().swap(unitCircleVertices);
    for(int i = 0; i <= m_sectorCount; ++i)
    {
        sectorAngle = i * sectorStep;
        unitCircleVertices.push_back(cos(sectorAngle)); // x
        unitCircleVertices.push_back(sin(sectorAngle)); // y
        unitCircleVertices.push_back(0);                // z
    }
}
