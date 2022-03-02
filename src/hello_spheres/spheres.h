#ifndef SPHERES_H
#define SPHERES_H

//https://github.com/caosdoar/spheres/blob/master/src/spheres.cpp


#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>


#include <glm/glm.hpp>

struct Mesh
{
    std::vector<glm::vec3> vertices;
    std::vector<uint32_t> triangles;

    uint32_t triangleCount() const { return triangles.size() / 3; }

    void addTriangle(uint32_t a, uint32_t b, uint32_t c)
    {
        triangles.emplace_back(a);
        triangles.emplace_back(b);
        triangles.emplace_back(c);
    }

    void addQuad(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
    {
        triangles.emplace_back(a);
        triangles.emplace_back(b);
        triangles.emplace_back(c);
        triangles.emplace_back(a);
        triangles.emplace_back(c);
        triangles.emplace_back(d);
    }

    void clear()
    {
        vertices.clear();
        triangles.clear();
    }

};

void UVSphere(uint32_t meridians, uint32_t parallels, Mesh &mesh);

void Icosahedron(Mesh &mesh);
void SubdivideMesh(const Mesh &meshIn, Mesh &meshOut);


#endif // SPHERES_H
