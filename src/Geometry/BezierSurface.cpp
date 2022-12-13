#include "BezierSurface.h"
#include "../Rendering/Renderer.h"
#include "Utils.h"

BezierSurface::BezierSurface( std::vector<std::vector<glm::vec3>> controlPoints,
                              unsigned int nbSegmentsU,
                              unsigned int nbSegmentsV,
                              glm::vec4 color ) :
    Mesh() {
    m_controlPoints     = controlPoints;
    m_nbSegmentsU       = nbSegmentsU;
    m_nbSegmentsV       = nbSegmentsV;
    unsigned int lignes = controlPoints.size();
    pascalCoeffs( lignes, m_coeffsLignes );
    unsigned int maxSize = 0;
    for ( unsigned int i = 0; i < lignes; i++ ) {
        unsigned int col = controlPoints[i].size();
        if ( col > maxSize ) maxSize = col;
    }
    pascalCoeffs( maxSize, m_coeffsCols );
    computeVertices( lignes, color );
    computeIndices();
    computeNormals();
    setupMesh( GL_TRIANGLES );
}

BezierSurface::~BezierSurface() {}

void BezierSurface::computeVertices( unsigned int lignes, glm::vec4 color ) {
    auto normal = glm::vec3( 0.f );
    int uf      = m_nbSegmentsU - 1;
    int vf      = m_nbSegmentsV - 1;
    for ( int ui = 0; ui <= uf; ui++ ) {
        float u = float( ui ) / uf;
        for ( int uj = 0; uj <= vf; uj++ ) {
            float v = float( uj ) / vf;
            std::vector<glm::vec3> uCurve;
            for ( unsigned int i = 0; i < lignes; i++ ) {
                uCurve.push_back( bezierValue( m_controlPoints[i], u, m_coeffsCols ) );
            }
            glm::vec3 puv = bezierValue( uCurve, v, m_coeffsLignes );
            Vertex vertex = { puv, normal, glm::vec2( u, v ), color };
            m_vertices.push_back( vertex );
            std::vector<unsigned int> vn;
            m_vertexNormals.push_back( vn );
        }
    }
}

void BezierSurface::computeIndices() {
    for ( unsigned int i = 0; i < m_nbSegmentsU - 1; i++ ) {
        for ( unsigned int j = 0; j < m_nbSegmentsV - 1; j++ ) {
            m_indices.push_back( i * m_nbSegmentsV + j );
            m_indices.push_back( i * m_nbSegmentsV + j + 1 );
            m_indices.push_back( ( i + 1 ) * m_nbSegmentsV + j + 1 );
            m_indices.push_back( i * m_nbSegmentsV + j );
            m_indices.push_back( ( i + 1 ) * m_nbSegmentsV + j + 1 );
            m_indices.push_back( ( i + 1 ) * m_nbSegmentsV + j );
        }
    }
}

void BezierSurface::computeNormals() {
    std::vector<glm::vec3> faceNormals;
    unsigned int faceId = 0;
    for ( unsigned int i = 0; i < m_indices.size(); i += 3 ) {
        unsigned int a   = m_indices[i];
        unsigned int b   = m_indices[i + 1];
        unsigned int c   = m_indices[i + 2];
        glm::vec3 edge1  = m_vertices[b].m_position - m_vertices[a].m_position;
        glm::vec3 edge2  = m_vertices[c].m_position - m_vertices[a].m_position;
        glm::vec3 normal = glm::cross( edge1, edge2 );
        faceNormals.push_back( normal );
        m_vertexNormals[a].push_back( faceId );
        m_vertexNormals[b].push_back( faceId );
        m_vertexNormals[c].push_back( faceId );
        faceId++;
    }
    for ( unsigned int i = 0; i < m_vertexNormals.size(); i++ ) {
        glm::vec3 sumN( 0.f );
        for ( unsigned int j = 0; j < m_vertexNormals[i].size(); j++ ) {
            sumN += faceNormals[m_vertexNormals[i][j]];
        }
        m_vertices[i].m_normal = glm::normalize( sumN );
    }
}
