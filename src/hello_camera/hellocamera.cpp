#include "hellocamera.h"
#include <iostream>
#include <math.h>
#include <vector>
#include "Geometry/BezierCurve.h"
#include "Geometry/BezierSurface.h"
#include "Geometry/Mesh.h"
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

SimpleCamera::SimpleCamera(int width, int height, ImVec4 clearColor) : OpenGLDemo(width, height, clearColor), _activecamera(0), _camera(nullptr) {
    // Initialise geometric data
    std::vector<glm::vec3> points1;
    std::vector<glm::vec3> points2;
    std::vector<glm::vec3> points3;
    std::vector<glm::vec3> points4;
    m_color = glm::vec4(1.0f, 0.5f, 0.2f, 1.f);
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
    points4.push_back(glm::vec3(2.0f, -1.3f, 1.5f));
    m_controlPoints.push_back(points1);
    m_controlPoints.push_back(points2);
    m_controlPoints.push_back(points3);
    m_controlPoints.push_back(points4);

    m_renderer = new Renderer();
    m_light.position = glm::vec3(2.f,1.f,4.f);
    m_light.color = glm::vec3(1.0f);

    Shader* program = 
        new Shader("/home/mafo/dev/helloOpenGL/Shaders/Camera.vert.glsl", "/home/mafo/dev/helloOpenGL/Shaders/Microfacet.frag.glsl");
    Shader* programNormal = 
        new Shader("/home/mafo/dev/helloOpenGL/Shaders/Camera.vert.glsl", "/home/mafo/dev/helloOpenGL/Shaders/Camera.frag.glsl");
    Shader* programParametric = 
        new Shader("/home/mafo/dev/helloOpenGL/Shaders/Camera.vert.glsl", "/home/mafo/dev/helloOpenGL/Shaders/Parametric.frag.glsl");

    m_material = new Material(program);
    m_materialNormal = new Material(programNormal);
    m_materialParametric = new Material(programParametric);
    
    compute();
    m_first = false;

    _cameraselector.push_back( []()->Camera*{return new EulerCamera(glm::vec3(0.f, 0.f, 3.f));} );
    _cameraselector.push_back( []()->Camera*{return new TrackballCamera(glm::vec3(0.f, 0.f, 3.f));} );

    _camera.reset(_cameraselector[_activecamera]());

    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _view = _camera->viewmatrix();

    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, 0.1f, 100.0f);

    _model = glm::translate(glm::mat4(1.0), m_translation);
    //m_renderer->setLight(m_light);
}

SimpleCamera::~SimpleCamera() {
    delete m_renderer;
    delete m_material;
    delete m_materialNormal;
    delete m_materialParametric;
}
void SimpleCamera::compute() {
    RenderObject* roCPM;
    if(m_displayCtrlPts){
        Vertex v0 = {m_controlPoints[0][0], glm::vec3(0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
        Vertex v1 = {m_controlPoints[0][1], glm::vec3(0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
        Vertex v2 = {m_controlPoints[0][2], glm::vec3(-0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
        Vertex v3 = {m_controlPoints[0][3], glm::vec3(-0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
        Vertex v4 = {m_controlPoints[1][0], glm::vec3(0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
        Vertex v5 = {m_controlPoints[1][1], glm::vec3(0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
        Vertex v6 = {m_controlPoints[1][2], glm::vec3(-0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
        Vertex v7 = {m_controlPoints[1][3], glm::vec3(-0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
        Vertex v8 = {m_controlPoints[2][0], glm::vec3(0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
        Vertex v9 = {m_controlPoints[2][1], glm::vec3(0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
        Vertex v10 = {m_controlPoints[2][2], glm::vec3(0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
        Vertex v11 = {m_controlPoints[2][3], glm::vec3(-0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
        Vertex v12 = {m_controlPoints[3][0], glm::vec3(-0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
        Vertex v13 = {m_controlPoints[3][1], glm::vec3(0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
        Vertex v14 = {m_controlPoints[3][2], glm::vec3(-0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
        Vertex v15 = {m_controlPoints[3][3], glm::vec3(-0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
        Vertex v16 = {m_controlPoints[3][4], glm::vec3(-0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f), m_color};
        std::vector<Vertex> vertices1 = { v0, v1, v2, v3, v4, v5, v6, v7, v8, v9,
                                          v10, v11, v12, v13, v14, v15, v16 };
        std::vector<unsigned int> indices1 = {
            0, 1, 1, 2, 2, 3,
            4, 5, 5, 6, 6, 7,
            8, 9, 9, 10, 10, 11,
            12, 13, 13, 14, 14, 15, 15, 16
        };
        std::vector<Texture> textures;
        Mesh* ctrlPtsMesh = new Mesh(vertices1, indices1, textures, GL_LINES);
        if(!m_first){
            roCPM = new RenderObject(ctrlPtsMesh, m_renderer->getCurrentMaterial());
        }
        else{
            roCPM = new RenderObject(ctrlPtsMesh, m_material);
        }

    }
    Mesh* mesh = new BezierSurface(m_controlPoints, 100, 100, m_color);
    RenderObject* ro;
    if(!m_first){
        ro = new RenderObject(mesh, m_renderer->getCurrentMaterial());
    }
    else{
        ro = new RenderObject(mesh, m_material);
    }
    m_renderer->clearRenderObjects();
    m_renderer->addRenderObject(ro);
    if(m_displayCtrlPts)
        m_renderer->addRenderObject(roCPM);
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

    m_renderer->setCameraPosition(_camera->position());
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
            m_renderer->setMaterial(m_material);
            return true;
        case 'n' :
            m_renderer->setMaterial(m_materialNormal);
            return true;
        case 't' :
            m_renderer->setMaterial(m_materialParametric);
            return true;
        default:
            return false;
    }
}
