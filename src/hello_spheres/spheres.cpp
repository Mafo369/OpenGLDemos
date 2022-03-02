#include "spheres.h"


void UVSphere(uint32_t meridians, uint32_t parallels, Mesh &mesh)
{
    mesh.vertices.emplace_back(0.0f, 1.0f, 0.0f);
    for (uint32_t j = 0; j < parallels - 1; ++j)
    {
        float polar = M_PI * float(j+1) / float(parallels);
        float sp = std::sin(polar);
        float cp = std::cos(polar);
        for (uint32_t i = 0; i < meridians; ++i)
        {
            float azimuth = 2.0 * M_PI * float(i) / float(meridians);
            float sa = std::sin(azimuth);
            float ca = std::cos(azimuth);
            float x = sp * ca;
            float y = cp;
            float z = sp * sa;
            mesh.vertices.emplace_back(x, y, z);
        }
    }
    mesh.vertices.emplace_back(0.0f, -1.0f, 0.0f);

    for (uint32_t i = 0; i < meridians; ++i)
    {
        uint32_t const a = i + 1;
        uint32_t const b = (i + 1) % meridians + 1;
        mesh.addTriangle(0, a, b);
    }

    for (uint32_t j = 0; j < parallels - 2; ++j)
    {
        uint32_t aStart = j * meridians + 1;
        uint32_t bStart = (j + 1) * meridians + 1;
        for (uint32_t i = 0; i < meridians; ++i)
        {
            const uint32_t a = aStart + i;
            const uint32_t a1 = aStart + (i + 1) % meridians;
            const uint32_t b = bStart + i;
            const uint32_t b1 = bStart + (i + 1) % meridians;
            mesh.addQuad(a, a1, b1, b);
        }
    }

    for (uint32_t i = 0; i < meridians; ++i)
    {
        uint32_t const a = i + meridians * (parallels - 2) + 1;
        uint32_t const b = (i + 1) % meridians + meridians * (parallels - 2) + 1;
        mesh.addTriangle(mesh.vertices.size() - 1, a, b);
    }
}

void Icosahedron(Mesh &mesh)
{
    const double t = (1.0 + std::sqrt(5.0)) / 2.0;

    // Vertices
    mesh.vertices.emplace_back(normalize(glm::vec3(-1.0,  t, 0.0)));
    mesh.vertices.emplace_back(normalize(glm::vec3( 1.0,  t, 0.0)));
    mesh.vertices.emplace_back(normalize(glm::vec3(-1.0, -t, 0.0)));
    mesh.vertices.emplace_back(normalize(glm::vec3( 1.0, -t, 0.0)));
    mesh.vertices.emplace_back(normalize(glm::vec3(0.0, -1.0,  t)));
    mesh.vertices.emplace_back(normalize(glm::vec3(0.0,  1.0,  t)));
    mesh.vertices.emplace_back(normalize(glm::vec3(0.0, -1.0, -t)));
    mesh.vertices.emplace_back(normalize(glm::vec3(0.0,  1.0, -t)));
    mesh.vertices.emplace_back(normalize(glm::vec3( t, 0.0, -1.0)));
    mesh.vertices.emplace_back(normalize(glm::vec3( t, 0.0,  1.0)));
    mesh.vertices.emplace_back(normalize(glm::vec3(-t, 0.0, -1.0)));
    mesh.vertices.emplace_back(normalize(glm::vec3(-t, 0.0,  1.0)));

    // Faces
    mesh.addTriangle(0, 11, 5);
    mesh.addTriangle(0, 5, 1);
    mesh.addTriangle(0, 1, 7);
    mesh.addTriangle(0, 7, 10);
    mesh.addTriangle(0, 10, 11);
    mesh.addTriangle(1, 5, 9);
    mesh.addTriangle(5, 11, 4);
    mesh.addTriangle(11, 10, 2);
    mesh.addTriangle(10, 7, 6);
    mesh.addTriangle(7, 1, 8);
    mesh.addTriangle(3, 9, 4);
    mesh.addTriangle(3, 4, 2);
    mesh.addTriangle(3, 2, 6);
    mesh.addTriangle(3, 6, 8);
    mesh.addTriangle(3, 8, 9);
    mesh.addTriangle(4, 9, 5);
    mesh.addTriangle(2, 4, 11);
    mesh.addTriangle(6, 2, 10);
    mesh.addTriangle(8, 6, 7);
    mesh.addTriangle(9, 8, 1);
}

void SubdivideMesh(const Mesh &meshIn, Mesh &meshOut)
{
    meshOut.vertices = meshIn.vertices;

    for (uint32_t i = 0; i < meshIn.triangleCount(); ++i)
    {
        const uint32_t f0 = meshIn.triangles[i * 3];
        const uint32_t f1 = meshIn.triangles[i * 3 + 1];
        const uint32_t f2 = meshIn.triangles[i * 3 + 2];

        const glm::vec3 v0 = meshIn.vertices[f0];
        const glm::vec3 v1 = meshIn.vertices[f1];
        const glm::vec3 v2 = meshIn.vertices[f2];

        const glm::vec3 v3 = glm::normalize(glm::vec3(0.5) * (v0 + v1));
        const uint32_t f3 = meshOut.vertices.size();
        meshOut.vertices.emplace_back(v3);
        const glm::vec3 v4 = glm::normalize(glm::vec3(0.5) * (v1 + v2));
        const uint32_t f4 = meshOut.vertices.size();
        meshOut.vertices.emplace_back(v4);
        const glm::vec3 v5 = glm::normalize(glm::vec3(0.5) * (v2 + v0));
        const uint32_t f5 = meshOut.vertices.size();
        meshOut.vertices.emplace_back(v5);

        meshOut.addTriangle(f0, f3, f5);
        meshOut.addTriangle(f3, f1, f4);
        meshOut.addTriangle(f4, f2, f5);
        meshOut.addTriangle(f3, f4, f5);
    }
}
