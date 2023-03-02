#include "hellocamera.h"
#include <iostream>
#include <math.h>
#include <memory>
#include <vector>
#include "Geometry/BezierCurve.h"
#include "Geometry/BezierSurface.h"
#include "Geometry/Cube.h"
#include "Geometry/Mesh.h"
#include "../../Rendering/RenderObject.h"

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
    /*** Initialise geometric data ***/
    std::vector<glm::vec3> points1;
    std::vector<glm::vec3> points2;
    std::vector<glm::vec3> points3;
    std::vector<glm::vec3> points4;
    m_color = glm::vec4(1.0f, 0.5f, 0.2f, 1.f);
    points1.push_back(glm::vec3(-0.5f,  -0.5f, 0.0f));
    points1.push_back(glm::vec3(0.f, 0.f, 0.0f));
    points1.push_back(glm::vec3(0.5f, 0.f, 0.0f));
    points1.push_back(glm::vec3(1.f, -0.5f, 0.0f));
    points2.push_back(glm::vec3(-0.5f,  -0.5f, 0.5f));
    points2.push_back(glm::vec3(0.f, 0.f, 0.5f));
    points2.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
    points2.push_back(glm::vec3(1.f, -0.5f, 0.5f));
    points3.push_back(glm::vec3(-0.5f,  -0.5f, 1.0f));
    points3.push_back(glm::vec3(0.f, 0.5f, 1.0f));
    points3.push_back(glm::vec3(0.83f, 1.3f, 1.0f));
    points3.push_back(glm::vec3(1.f, -0.3f, 1.0f));
    points4.push_back(glm::vec3(-0.5f,  0.f, 1.5f));
    points4.push_back(glm::vec3(0.f, 0.f, 1.5f));
    points4.push_back(glm::vec3(0.5f, -1.0f, 1.5f));
    points4.push_back(glm::vec3(1.f, 0.3f, 1.5f));
    m_controlPoints.push_back(points1);
    m_controlPoints.push_back(points2);
    m_controlPoints.push_back(points3);
    m_controlPoints.push_back(points4);

    /*** Initialise renderer ***/
    m_renderer = new Renderer();

    /*** Create render objects ***/

    // Textures
    Texture* texture = new Texture("Assets/container2.png");
    Texture* textureSpecular = new Texture("Assets/container2_specular.png");
    
    // Shaders
    Shader* program = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/Microfacet.frag.glsl");
    Shader* programModified = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/MicrofacetModified.frag.glsl");
    Shader* programLambert = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/Lambert.frag.glsl");
    Shader* programNormal = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/Normals.frag.glsl");
    Shader* programParametric = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/Parametric.frag.glsl");
    Shader* programTexture = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/MicrofacetTexture.frag.glsl");

    // Materials
    MaterialParams matParams;
    matParams.texDiffuse = 0;
    matParams.texSpecular = 1;
    matParams.metallic = 0.6;
    matParams.roughness = 0.6;
    m_material = std::make_shared<Material>(program, matParams);
    m_materialModified = std::make_shared<Material>(programModified, matParams);
    m_materialLambert = std::make_shared<Material>(programLambert);
    m_materialNormal = std::make_shared<Material>(programNormal);
    m_materialParametric = std::make_shared<Material>(programParametric);
    m_materialTexture = std::make_shared<Material>(programTexture, matParams, texture, textureSpecular);
    
    m_currentMaterial = m_material;
    
    // Render objects
    compute();
    m_first = false;

    /*** Create Camera ***/
    _cameraselector.push_back( []()->Camera*{return new EulerCamera(glm::vec3(0.f, 0.f, 3.f));} );
    _cameraselector.push_back( []()->Camera*{return new TrackballCamera(glm::vec3(0.f, 0.f, 3.f));} );
    _camera.reset(_cameraselector[_activecamera]());
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _view = _camera->viewmatrix();
    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, 0.1f, 100.0f);
    _model = glm::translate(glm::mat4(1.0), m_translation);

    /*** Create lights ***/
    LightParams lightParams;
    lightParams.position = glm::vec3(2.f, 5.f, 2.9f);
    lightParams.color = glm::vec3(1.0f);
    Light* light = new Light(lightParams);
    
    lightParams.position = glm::vec3(0.95f, 1.43f, 1.28f);
    Light* light1 = new Light(lightParams);

    lightParams.position = glm::vec3(-2.f, 0.3, 0.0f);
    Light* light2 = new Light(lightParams);

    /*** Attach lights to renderer ***/
    m_renderer->addLightRo(light);
    m_renderer->addLightRo(light1);
    m_renderer->addLightRo(light2);
    m_lights.push_back(light);
    m_lights.push_back(light1);
    m_lights.push_back(light2);
}

SimpleCamera::~SimpleCamera() {
    delete m_renderer;
    for(auto& l : m_lights)
        delete l;
}

void SimpleCamera::compute() {
    /*** Create control points render objects if needed ***/
    RenderObject* roCPM;
    if(m_displayCtrlPts){
        std::vector<Vertex> vertices1;
        for(auto& points : m_controlPoints){
            for(auto& point : points){
                Vertex v = {point, glm::vec3(0.f), glm::vec2(0.f), m_color};
                vertices1.push_back(v);
            }
        } 
        std::vector<unsigned int> indices1;
        unsigned int id = 0;
        for(unsigned int i = 0; i < m_controlPoints.size(); i++){
            for(unsigned int j = 0; j < m_controlPoints[i].size()-1; j++){
                indices1.push_back(id);
                id++;
                indices1.push_back(id);
            }
            id++;
        }
        Mesh* ctrlPtsMesh = new Mesh(vertices1, indices1, GL_LINES);
        if(!m_first){
            roCPM = new RenderObject(ctrlPtsMesh, m_renderer->getCurrentMaterial());
        }
        else{
            roCPM = new RenderObject(ctrlPtsMesh, m_material);
        }
    }

    /*** Create Bezier surface render object ***/
    Mesh* mesh = new BezierSurface(m_controlPoints, 100, 100, m_color);
    RenderObject* ro;
    if(!m_first){
        ro = new RenderObject(mesh, m_renderer->getCurrentMaterial());
    }
    else{
        ro = new RenderObject(mesh, m_material);
    }

    /*** Delete old render objects and add new ones ***/
    m_renderer->clearRenderObjects();
    m_renderer->addRenderObject(ro);
    if(m_displayCtrlPts)
        m_renderer->addRenderObject(roCPM);
    glm::vec4 planeColor = {0.5,0.5,0.5,1};
    Vertex v0P = {glm::vec3(-5., -2.f,  5.), glm::vec3(0,1,0), glm::vec2(0.f, 1.f), planeColor};
    Vertex v1P = {glm::vec3(-5., -2.f, -5.), glm::vec3(0,1,0), glm::vec2(0.f, 0.f), planeColor};
    Vertex v2P = {glm::vec3(5., -2.f, -5.), glm::vec3(0,1,0), glm::vec2(1.f, 0.f), planeColor};
    Vertex v3P = {glm::vec3(-5., -2.f, 5.), glm::vec3(0,1,0), glm::vec2(0.f, 1.f), planeColor};
    Vertex v4P = {glm::vec3(5., -2.f, -5.), glm::vec3(0,1,0), glm::vec2(1.f, 0.f), planeColor};
    Vertex v5P = {glm::vec3(5., -2.f, 5.), glm::vec3(0,1,0), glm::vec2(1.f, 1.f), planeColor};
    std::vector<Vertex> verticesPlane = { v0P, v1P, v2P, v3P, v4P, v5P };
    std::vector<unsigned int> indicesPlane = {
        0, 1, 2,   // First Triangle
        3, 4, 5    // Second Triangle
    };
    auto planeMesh = new Mesh(verticesPlane, indicesPlane, GL_TRIANGLES);
    auto planeRo = new RenderObject(planeMesh, m_material);
    m_renderer->addRenderObject(planeRo);
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

    /*** Compute new camera transform ***/
    _model = glm::translate(glm::mat4(1.0), m_translation);
    _view = _camera->viewmatrix();
    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, 0.1f, 100.0f);

    /*** Update Material ***/
    m_currentMaterial = m_renderer->getCurrentMaterial();
    m_renderer->setMaterialParams();
   
    /*** Update scene ***/
    m_renderer->setVP(_view, _projection);
    m_renderer->setCameraPosition(_camera->position());
    for(unsigned int i = 0; i < m_lights.size(); i++){
        auto& l = m_lights[i];
        l->update(m_translation);
        m_renderer->setLightMVP(l->getModel(), _view, _projection , i);
        m_renderer->setLight(l, i);
    }

    /*** Draw new scene ***/
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
        case 'm' :
            m_renderer->setMaterial(m_materialModified);
            return true;
        case 'n' :
            m_renderer->setMaterial(m_materialNormal);
            return true;
        case 'u' :
            m_renderer->setMaterial(m_materialParametric);
            return true;
        case 'l' :
            m_renderer->setMaterial(m_materialLambert);
            return true;
        case 't' :
            m_renderer->setMaterial(m_materialTexture);
            return true;
        default:
            return false;
    }
}
