#include "hellobloom.h"
#include <iostream>
#include <math.h>
#include <memory>
#include <vector>
#include "Geometry/BezierCurve.h"
#include "Geometry/BezierSurface.h"
#include "Geometry/Cube.h"
#include "Geometry/Mesh.h"
#include "../../Rendering/RenderObject.h"
#include "../../../libs/stb_image.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/


#define deg2rad(x) float(M_PI)*(x)/180.f

BloomDemo::BloomDemo(int width, int height, ImVec4 clearColor) : OpenGLDemo(width, height, clearColor), _activecamera(0), _camera(nullptr) {
    /*** Initialise geometric data ***/
    m_color = glm::vec4(1.0f, 0.5f, 0.2f, 1.f);

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
    Shader* programQuad = 
        new Shader("Shaders/Quad.vert.glsl", "Shaders/Quad.frag.glsl");
    Shader* programTh = 
        new Shader("Shaders/Quad.vert.glsl", "Shaders/Threshold.frag.glsl");

    programQuad->bind();
    programQuad->setUniform1i("screenTexture", 0);
    programQuad->setUniform1i("exposure", m_exposure);
    programQuad->unbind();

    programQuad->bind();
    programQuad->setUniform1i("screenTexture", 0);
    programQuad->setUniform1i("threshold", m_threshold);
    programQuad->unbind();

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
    m_materialQuad = std::make_shared<Material>(programQuad);
    m_materialTh = std::make_shared<Material>(programTh);
    
    m_currentMaterial = m_materialQuad;
    
    // Render objects
    compute();
    m_first = false;

    /*** Create Camera ***/
    _cameraselector.push_back( []()->Camera*{return new EulerCamera(glm::vec3(0.f, 0.f, 1.f));} );
    _cameraselector.push_back( []()->Camera*{return new TrackballCamera(glm::vec3(0.f, 0.f, 1.f));} );
    _camera.reset(_cameraselector[_activecamera]());
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _view = _camera->viewmatrix();
    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, 0.0f, 1.0f);
    _model = glm::translate(glm::mat4(1.0), m_translation);

    /*** Create lights ***/
    LightParams lightParams;
    lightParams.position = glm::vec3(0.f, 0.f, 0.f);
    lightParams.color = glm::vec3(1.1f);
    Light* light = new Light(lightParams);
    
    /*** Attach lights to renderer ***/
    m_renderer->addLightRo(light);
    m_lights.push_back(light);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    stbi_set_flip_vertically_on_load(true);
    int hdrWidth, hdrHeight, nrComponents;
    float *data = stbi_loadf("/home/mafo/dev/Raytracer/assets/rainforest_trail_4k.hdr", &hdrWidth, &hdrHeight, &nrComponents, 0);
    if (!data)
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }  

    // generate texture
    glGenTextures(1, &m_fboTexture);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, hdrWidth, hdrHeight, 0, GL_RGB, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTexture, 0);

    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo); 
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, hdrWidth, hdrHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  

    Vertex v0 = {normalize(glm::vec3(-1.0f,  1.0f, 0.0f)), glm::vec3(0), glm::vec2(0.f, 1.f), m_color};
    Vertex v1 = {normalize(glm::vec3(-1.0f, -1.0f, 0.0f)), glm::vec3(0), glm::vec2(0.f, 0.f), m_color};
    Vertex v2 = {normalize(glm::vec3(1.0f, -1.0f, 0.0f)), glm::vec3(0), glm::vec2(1.f, 0.f), m_color};
    Vertex v3 = {normalize(glm::vec3(-1.0f, 1.0f, 0.0f)), glm::vec3(0), glm::vec2(0.f, 1.f), m_color};
    Vertex v4 = {normalize(glm::vec3(1.0f, -1.0f, 0.0f)), glm::vec3(0), glm::vec2(1.f, 0.f), m_color};
    Vertex v5 = {normalize(glm::vec3(1.0f, 1.0f, 0.0f)), glm::vec3(0), glm::vec2(1.f, 1.f), m_color};
    std::vector<Vertex> vertices = { v0, v1, v2, v3, v4, v5 };
    std::vector<unsigned int> indices = {
        0, 1, 2,   // First Triangle
        3, 4, 5    // Second Triangle
    };
 
    m_mesh = new Mesh(vertices, indices, GL_TRIANGLES);

    stbi_image_free(data);
}

BloomDemo::~BloomDemo() {
    delete m_renderer;
    for(auto& l : m_lights)
        delete l;
}

void BloomDemo::compute() {
    m_materialQuad->getShader()->bind();
    m_materialQuad->getShader()->setUniform1f("exposure", m_exposure);
    m_materialQuad->getShader()->unbind();

    m_materialTh->getShader()->bind();
    m_materialTh->getShader()->setUniform1f("threshold", m_threshold);
    m_materialTh->getShader()->unbind();
}

void BloomDemo::resize(int width, int height){
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

void BloomDemo::draw() {
    OpenGLDemo::draw();

    // first pass
    //glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    //glEnable(GL_DEPTH_TEST);
    //m_renderer->draw();	
      
    // second pass
    glBindFramebuffer(GL_FRAMEBUFFER, 1); // back to default
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.9f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    glBindTexture(GL_TEXTURE_2D, m_fboTexture);

    m_renderer->draw(m_mesh, m_currentMaterial->getShader());

    /*** Compute new camera transform ***/
    _model = glm::translate(glm::mat4(1.0), m_translation);
    _view = _camera->viewmatrix();
    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, 0.1f, 100.0f);

    /*** Update Material ***/
    if(m_renderer->getCurrentMaterial() != nullptr)
      m_currentMaterial = m_renderer->getCurrentMaterial();
    m_renderer->setMaterialParams();
   
    /*** Update scene ***/
    m_renderer->setMVP(_model, _view, _projection);
    m_renderer->setCameraPosition(_camera->position());
    for(unsigned int i = 0; i < m_lights.size(); i++){
        auto& l = m_lights[i];
        l->update(m_translation);
        m_renderer->setLightMVP(l->getModel(), _view, _projection , i);
        m_renderer->setLight(l, i);
    }

}

void BloomDemo::mouseclick(int button, float xpos, float ypos) {
    _button = button;
    _mousex = xpos;
    _mousey = ypos;
    _camera->processmouseclick(_button, xpos, ypos);
}

void BloomDemo::mousemove(float xpos, float ypos) {
    _camera->processmousemovement(_button, xpos, ypos, true);
}

void BloomDemo::mousewheel(float delta) {
    _camera->processmousescroll(delta);
}

void BloomDemo::keyboardmove(int key, double time) {
    _camera->processkeyboard(Camera_Movement(key), time);
}

bool BloomDemo::keyboard(unsigned char k) {
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
        case 'h' :
            m_currentMaterial = m_materialTh;
            return true;
        case 'q' :
            m_currentMaterial = m_materialQuad;
            return true;
        default:
            return false;
    }
}
