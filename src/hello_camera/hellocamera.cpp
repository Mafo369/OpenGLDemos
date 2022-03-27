#include "hellocamera.h"
#include <iostream>
#include <math.h>
#include <vector>
#include "Geometry/BezierCurve.h"
#include "Geometry/BezierSurface.h"
#include "RenderObject.h"

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
    m_controlPoints.push_back(points1);
    m_controlPoints.push_back(points2);
    m_controlPoints.push_back(points3);
    m_controlPoints.push_back(points4);
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
 
    m_renderer = new Renderer();

    //m_mesh = new BezierCurve(points1, 100);

    m_program = 
        new Shader("/home/mafo/dev/helloOpenGL/Shaders/Camera.vert.glsl", "/home/mafo/dev/helloOpenGL/Shaders/Basic.frag.glsl");
    m_programNormal = 
        new Shader("/home/mafo/dev/helloOpenGL/Shaders/Camera.vert.glsl", "/home/mafo/dev/helloOpenGL/Shaders/Camera.frag.glsl");
    m_programParametric = 
        new Shader("/home/mafo/dev/helloOpenGL/Shaders/Camera.vert.glsl", "/home/mafo/dev/helloOpenGL/Shaders/Parametric.frag.glsl");
    
    compute();
    m_first = false;

    _cameraselector.push_back( []()->Camera*{return new EulerCamera(glm::vec3(0.f, 0.f, 3.f));} );
    _cameraselector.push_back( []()->Camera*{return new TrackballCamera(glm::vec3(0.f, 0.f, 3.f));} );

    _camera.reset(_cameraselector[_activecamera]());

    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _view = _camera->viewmatrix();

    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, 0.1f, 100.0f);

    _model = glm::translate(glm::mat4(1.0), m_translation);
}

SimpleCamera::~SimpleCamera() {
    delete m_renderer;
    delete m_program;
    delete m_programNormal;
}
void SimpleCamera::compute() {
    Mesh* mesh = new BezierSurface(m_controlPoints, 100, 100);
    RenderObject* ro;
    if(!m_first)
        ro = new RenderObject(mesh, m_renderer->getCurrentShader());
    else
        ro = new RenderObject(mesh, m_program);
    m_renderer->clearRenderObjects();
    m_renderer->addRenderObject(ro);
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

    m_renderer->setMVP(_model, _view, _projection);
    m_renderer->draw();
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
        case 'c' :
            m_renderer->setShader(m_program);
            return true;
        case 'n' :
            m_renderer->setShader(m_programNormal);
            return true;
        case 't' :
            m_renderer->setShader(m_programParametric);
            return true;
        default:
            return false;
    }
}
