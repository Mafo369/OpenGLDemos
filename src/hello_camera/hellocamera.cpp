#include "hellocamera.h"
#include <iostream>
#include <math.h>
#include <vector>
#include "Geometry/BezierCurve.h"
#include "Geometry/BezierSurface.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/


#define deg2rad(x) float(M_PI)*(x)/180.f

//void pascalCoeffs(int n, std::vector<std::vector<int>>& coeffs){
//    for(int line = 1; line <= n; line++){
//        int C = 1;
//        std::vector<int> coeffI;
//        for(int i = 1; i <= line; i++){
//            coeffI.push_back(C);
//            C = C * (line - i) / i;
//        }
//        coeffs.push_back(coeffI);
//    }
//}
//
//glm::vec3 bezierValue(std::vector<glm::vec3>& p, float t, std::vector<std::vector<int>>& coeffs){
//    glm::vec3 pu(0.0f);
//    int n = p.size()-1;
//    for(int i = 0; i <= n; i++){
//        float Bi = coeffs[n][i] * powf(t, i) * powf(1.0f-t, n-i);  
//        pu += Bi * p[i];
//    }
//    return pu;
//}

SimpleCamera::SimpleCamera(int width, int height, ImVec4 clearColor) : OpenGLDemo(width, height, clearColor), _activecamera(0), _camera(nullptr) {
    // Initialise geometric data
    std::vector<std::vector<glm::vec3>> points;
    std::vector<glm::vec3> points1;
    std::vector<glm::vec3> points2;
    std::vector<glm::vec3> points3;
    std::vector<glm::vec3> points4;
    points1.push_back(glm::vec3(0.f,  -0.5f, 0.0f));
    points1.push_back(glm::vec3(0.5f, 0.f, 0.0f));
    points1.push_back(glm::vec3(1.f, 0.f, 0.0f));
    points1.push_back(glm::vec3(1.5f, -0.5f, 0.0f));
    points2.push_back(glm::vec3(0.f,  -0.5f, 0.5f));
    points2.push_back(glm::vec3(0.5f, 0.f, 0.5f));
    points2.push_back(glm::vec3(1.f, 0.5f, 0.5f));
    points2.push_back(glm::vec3(1.5f, -0.5f, 0.5f));
    points3.push_back(glm::vec3(0.f,  -0.5f, 1.0f));
    points3.push_back(glm::vec3(0.5f, 0.5f, 1.0f));
    points3.push_back(glm::vec3(1.f, 1.0f, 1.0f));
    points3.push_back(glm::vec3(1.5f, -0.3f, 1.0f));
    points4.push_back(glm::vec3(0.f,  1.5f, 1.5f));
    points4.push_back(glm::vec3(0.5f, 0.f, 1.5f));
    points4.push_back(glm::vec3(1.f, -1.0f, 1.5f));
    points4.push_back(glm::vec3(1.5f, 0.3f, 1.5f));
    points.push_back(points1);
    points.push_back(points2);
    points.push_back(points3);
    points.push_back(points4);
    Vertex v0 = {points1[0], glm::vec3(0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v1 = {points1[1], glm::vec3(0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v2 = {points1[2], glm::vec3(-0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v3 = {points1[3], glm::vec3(-0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v4 = {points2[0], glm::vec3(0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v5 = {points2[1], glm::vec3(0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v6 = {points2[2], glm::vec3(-0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v7 = {points2[3], glm::vec3(-0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v8 = {points3[0], glm::vec3(0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v9 = {points3[1], glm::vec3(0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v10 = {points3[2], glm::vec3(0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v11 = {points3[3], glm::vec3(-0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v12 = {points4[0], glm::vec3(-0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v13 = {points4[1], glm::vec3(0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v14 = {points4[2], glm::vec3(-0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v15 = {points4[3], glm::vec3(-0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    std::vector<Vertex> vertices1 = { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9,
                                      v10, v11, v12, v13, v14, v15 };
    std::vector<unsigned int> indices1 = {
        0, 1, 1, 2, 2, 3,
        4, 5, 5, 6, 6, 7,
        8, 9, 9, 10, 10, 11,
        12, 13, 13, 14, 14, 15
    };

    std::vector<Texture> textures;

    // ===================== BEZIER SURFACE ===========================//

    //int lignes = points.size();
    //int cols = points[0].size();
    //std::vector<std::vector<int>> coeffsLignes;
    //std::vector<std::vector<int>> coeffsCols;
    //pascalCoeffs(lignes, coeffsLignes);
    //pascalCoeffs(cols, coeffsCols);

    //std::vector<Vertex> vertices;
    //std::vector<unsigned int> indices;
    //std::vector<std::vector<unsigned int>> vertexNormals;
    //auto normal = glm::vec3(0.f);
    //int n = 100; // Nb segments en u
    //int m = 100; // Nb segments en v
    //int uf = n-1;
    //int vf = m-1;
    //for(int ui = 0; ui <= uf; ui++){
    //    float u = float(ui)/uf;
    //    for(int uj = 0; uj <= vf; uj++){
    //        float v = float(uj)/vf;
    //        std::vector<glm::vec3> uCurve;
    //        for(int i = 0; i < lignes; i++){
    //            uCurve.push_back(bezierValue(points[i], u, coeffsCols)); 
    //        }
    //        glm::vec3 puv = bezierValue(uCurve, v, coeffsLignes);
    //        Vertex vertex = { puv, normal, glm::vec2(0.f, 0.f) };
    //        vertices.push_back(vertex);
    //        std::vector<unsigned int> vn;
    //        vertexNormals.push_back(vn);
    //    }
    //}
    //for(int i = 0; i < n-1; i++){
    //    for(int j = 0; j < m-1; j++){
    //        indices.push_back(i*m+j);
    //        indices.push_back(i*m+j+1);
    //        indices.push_back((i+1)*m+j+1);
    //        indices.push_back(i*m+j);
    //        indices.push_back((i+1)*m+j+1);
    //        indices.push_back((i+1)*m+j);
    //    }
    //}
    //std::vector<glm::vec3> faceNormals;
    //unsigned int faceId = 0;
    //for(unsigned int i = 0; i < indices.size(); i += 3){
    //    unsigned int a = indices[i];
    //    unsigned int b = indices[i+1];
    //    unsigned int c = indices[i+2];
    //    glm::vec3 edge1 = vertices[b].m_position - vertices[a].m_position;
    //    glm::vec3 edge2 = vertices[c].m_position - vertices[a].m_position;
    //    glm::vec3 normal = glm::cross(edge1, edge2);
    //    faceNormals.push_back(normal);
    //    vertexNormals[a].push_back(faceId);
    //    vertexNormals[b].push_back(faceId);
    //    vertexNormals[c].push_back(faceId);
    //    faceId++;
    //}
    //for(unsigned int i = 0; i < vertexNormals.size(); i++){
    //    glm::vec3 sumN(0.f);
    //    for(unsigned int j = 0; j < vertexNormals[i].size(); j++){
    //        sumN += faceNormals[vertexNormals[i][j]];
    //    }
    //    vertices[i].m_normal = glm::normalize(sumN);
    //}

    // ===================== BEZIER CURVE ===========================//
     
    //int indice = 0;
    //std::vector<Vertex> vertices;
    //std::vector<unsigned int> indices;
    //auto normal = glm::vec3(0.577350269189626f, 0.0, 0.0);
    //int n = 100; // Nb de segments en u
    //int uf = n-1;
    //for(int j = 0; j <= uf; j++){
    //    float u = float(j)/uf;
    //    glm::vec3 pu = bezierValue(points1, u, coeffsCols);
    //    indices.push_back(indice);
    //    indice++;
    //    indices.push_back(indice);
    //    Vertex v = { pu, normal, glm::vec2(0.f, 0.f) };
    //    vertices.push_back(v);
    //}
    //indices.pop_back();
 
    m_renderer = new Renderer();

    //m_mesh = new BezierCurve(points1, 100);
    m_mesh = new BezierSurface(points, 100, 100);
    //m_mesh = new Mesh(vertices, indices, textures, GL_TRIANGLES);
    //m_meshL = new Mesh(vertices1, indices1, textures, GL_LINES);

    m_program = 
        new Shader("/home/mafo/dev/helloOpenGL/Shaders/Camera.vert.glsl", "/home/mafo/dev/helloOpenGL/Shaders/Camera.frag.glsl");


    _cameraselector.push_back( []()->Camera*{return new EulerCamera(glm::vec3(0.f, 0.f, 3.f));} );
    _cameraselector.push_back( []()->Camera*{return new TrackballCamera(glm::vec3(0.f, 0.f, 3.f));} );

    _camera.reset(_cameraselector[_activecamera]());

    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _view = _camera->viewmatrix();

    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, 0.1f, 100.0f);

    _model = glm::translate(glm::mat4(1.0), m_translation);
}

SimpleCamera::~SimpleCamera() {
    delete m_mesh;
    delete m_renderer;
}

void SimpleCamera::resize(int width, int height){
    OpenGLDemo::resize(width, height);
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
}

template <typename T>
void printmatrix(T * ptr) {
    std::cerr << std::fixed;
    for (int i=0; i<4; ++i) {
        std::cerr << "( ";
        for (int j=0; j<4; ++j) {
            std::cerr << *ptr++ << " ";
        }
        std::cerr << ") ";
    }
    std::cerr  << std::defaultfloat << std::endl;
}

void SimpleCamera::draw() {
    OpenGLDemo::draw();

    _model = glm::translate(glm::mat4(1.0), m_translation);
    _view = _camera->viewmatrix();
    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, 0.1f, 100.0f);

    m_program->setMVP(_model, _view, _projection);
    m_renderer->draw(m_mesh, m_program);
    //m_renderer->draw(m_meshL, m_program);
}

void SimpleCamera::mouseclick(int button, float xpos, float ypos) {
    _button = button;
    _mousex = xpos;
    _mousey = ypos;
    _camera->processmouseclick(_button, xpos, ypos);
}

void SimpleCamera::mousemove(float xpos, float ypos) {
    _camera->processmousemovement(_button, xpos, ypos, true);
}

void SimpleCamera::mousewheel(float delta) {
    _camera->processmousescroll(delta);
}

void SimpleCamera::keyboardmove(int key, double time) {
    _camera->processkeyboard(Camera_Movement(key), time);
}

bool SimpleCamera::keyboard(unsigned char k) {
    switch(k) {
        case 'p':
            _activecamera = (_activecamera+1)%2;
            _camera.reset(_cameraselector[_activecamera]());
            _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
            return true;
        default:
            return false;
    }
}
