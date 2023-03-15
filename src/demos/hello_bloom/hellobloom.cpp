#include "hellobloom.h"
#include <iostream>
#include <math.h>
#include <memory>
#include <vector>
#include "Geometry/BezierCurve.h"
#include "Geometry/BezierSurface.h"
#include "Geometry/Cube.h"
#include "Geometry/Sphere.h"
#include "Geometry/Plane.h"
#include "Geometry/Mesh.h"
#include "Geometry/Cylinder.h"
#include "Rendering/Bone.h"
#include "Rendering/RenderObject.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>

#define deg2rad(x) float(M_PI)*(x)/180.f

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../../libs/stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

BloomDemo::BloomDemo(int width, int height, ImVec4 clearColor) : OpenGLDemo(width, height, clearColor), _camera(nullptr), _activecamera(0) {
    /*** Initialise geometric data ***/
    m_color = glm::vec4(45.1f, 0.3f, 0.3f, 1.f);

    /*** Initialise renderer ***/
    m_renderer = new Renderer();
    m_renderer->setEnvMap("Assets/Alexs_Apt_2k.hdr");

    // Textures
    Texture* texture = new Texture("Assets/container2.png");
    Texture* textureSpecular = new Texture("Assets/container2_specular.png");
    Texture* texNormal = new Texture(glm::vec3(0));
    Texture* texAO = new Texture(glm::vec3(0));
    Texture* texE = new Texture(glm::vec3(0));
    
    // Shaders
    Shader* program = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/ShadowMapTest.frag.glsl");
    Shader* programMicrofacet = 
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
    Shader* programBasic = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/Basic.frag.glsl");
    Shader* programSpecular = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/Specular.frag.glsl");
    Shader* programWeight = 
        new Shader("Shaders/anim.vert.glsl", "Shaders/Weight.frag.glsl");
    m_programQuad = 
        new Shader("Shaders/Sample.vert.glsl", "Shaders/Quad.frag.glsl");
    m_programTh = 
        new Shader("Shaders/Sample.vert.glsl", "Shaders/Threshold.frag.glsl");
    m_programDown = 
        new Shader("Shaders/Sample.vert.glsl", "Shaders/Downsample.frag.glsl");
    m_programUp = 
        new Shader("Shaders/Sample.vert.glsl", "Shaders/Upsample.frag.glsl");

    m_programQuad->bind();
    m_programQuad->setUniform1i("screenTexture", 0);
    m_programQuad->setUniform1i("origTexture", 1);
    m_programQuad->setUniform1f("exposure", m_exposure);
    m_programQuad->unbind();

    m_programTh->bind();
    m_programTh->setUniform1i("screenTexture", 0);
    m_programTh->setUniform1f("threshold", m_threshold);
    m_programTh->unbind();

    m_programDown->bind();
    m_programDown->setUniform1i("srcTexture", 0);
    m_programDown->unbind();

    m_programUp->bind();
    m_programUp->setUniform1i("srcTexture", 0);
    m_programUp->unbind();

    // Materials
    MaterialParams matParams;
    matParams.texDiffuse = 2;
    matParams.texSpecular = 3;
    matParams.texNormal = 4;
    matParams.texAO = 5;
    matParams.metallic = 0.939;
    matParams.roughness = 0.276;
    m_material = std::make_shared<Material>(program, matParams);
    m_materialSpecular = std::make_shared<Material>(programSpecular, matParams);
    m_materialMicrofacet = std::make_shared<Material>(programMicrofacet, matParams);
    m_materialBasic = std::make_shared<Material>(programBasic, matParams);
    m_materialModified = std::make_shared<Material>(programModified, matParams);
    m_materialLambert = std::make_shared<Material>(programLambert);
    m_materialNormal = std::make_shared<Material>(programNormal);
    m_materialParametric = std::make_shared<Material>(programParametric);
    m_materialTexture = std::make_shared<Material>(programTexture, matParams, texture, textureSpecular, texNormal, texAO, texE);
    m_materialWeight = std::make_shared<Material>(programWeight, matParams);
    
    m_first = false;

    /*** Create Camera ***/
    _cameraselector.push_back( []()->Camera*{return new EulerCamera(glm::vec3(-10.09f, 2.47f, 7.6f), {0,1.f,0.f}, -50.f, 0.f);} );
    _cameraselector.push_back( []()->Camera*{return new TrackballCamera(glm::vec3(0.f, 0.f, 1.f));} );
    _camera.reset(_cameraselector[_activecamera]());
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _view = _camera->viewmatrix();
    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, _camera->getNearPlane(), _camera->getFarPlane());

    /*** Add lights to renderer ***/
    m_renderer->addPointLight(glm::vec3(3.f, 6.f, 0.f), glm::vec3(50));
    m_renderer->addPointLight(glm::vec3(0.f, 6.f, 0.f), glm::vec3(15.0f, 0, 0));
    m_renderer->addPointLight(glm::vec3(-3.f, 6.f, 0.0f), glm::vec3(0.0f, 15.0f, 0.f));
    m_renderer->addPointLight(glm::vec3(-6.f, 6.f, 0.0f), glm::vec3(0.0f, 0.0f, 25.f));
    m_renderer->addPointLight(glm::vec3(0.f, 1.f, 0.0f), glm::vec3(5.f));

    programTexture->bind();
    programTexture->setUniform1i("nLights", 5);
    programTexture->unbind();
    m_renderer->setDirLight(lightDir, glm::vec3(1.f));


    /*** Setup Framebuffer for original render and threshold ***/
    m_fbo = new Framebuffer();
    m_fbo->bind();
    
    int hdrWidth = _width;
    int hdrHeight = _height;

    // Original render texture
    glGenTextures(1, &m_fboTexture);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, hdrWidth, hdrHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach it to currently bound framebuffer object
    m_fbo->attachTexture2D(GL_COLOR_ATTACHMENT0, m_fboTexture);

    // Thresholded texture of original used for Bloom
    glGenTextures(1, &m_fboThTexture);
    glBindTexture(GL_TEXTURE_2D, m_fboThTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, hdrWidth, hdrHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach it to currently bound framebuffer object
    m_fbo->attachTexture2D(GL_COLOR_ATTACHMENT1, m_fboThTexture);

    // create a renderbuffer object for depth and stencil attachment
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, hdrWidth, hdrHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
    m_fbo->attachRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    m_fbo->unbind();

    /*** Setup Bloom Framebuffer for final Bloom image ***/
    m_mipfbo = new Framebuffer();
    m_mipfbo->bind();

    glm::vec2 mipSize((float)hdrWidth, (float)hdrHeight);
    glm::ivec2 mipIntSize((int)hdrWidth, (int)hdrHeight);
    // Safety check
    if (hdrWidth > (int)INT_MAX || hdrHeight > (int)INT_MAX) {
        std::cerr << "Window size conversion overflow - cannot build bloom FBO!\n";
    }

    // Create different mips for Bloom texture
    for(unsigned int i = 0; i < 6; i++){
      BloomMip mip;

      mipSize *= 0.5f;
      mipIntSize /= 2;
      mip.size = mipSize;
      mip.intSize = mipIntSize;

      // generate texture
      glGenTextures(1, &mip.texture);
      glBindTexture(GL_TEXTURE_2D, mip.texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, (int)mipSize.x, (int)mipSize.y, 0, GL_RGB, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

      std::cout << "Created bloom mip " << mipIntSize.x << 'x' << mipIntSize.y << std::endl;
      m_mipChain.emplace_back(mip);
    }

    // attach first mip to currently bound framebuffer object
    m_mipfbo->attachTexture2D(GL_COLOR_ATTACHMENT0, m_mipChain[0].texture);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    m_mipfbo->unbind();

    /*** Add render objects of scene ***/
    auto monkeyTransform = glm::translate(glm::mat4(1.f), glm::vec3(0, 3, -4)) * glm::scale(glm::mat4(1.f), glm::vec3(0.68f));
    m_renderer->loadModel("Assets/Suzanne.obj", programBasic, m_color, monkeyTransform);

    auto helmetTransform = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -3)) * glm::rotate(glm::mat4(1.f), deg2rad(90), glm::vec3(1,0,0));
    m_renderer->loadModel("Assets/DamagedHelmet/glTF/DamagedHelmet.gltf", programTexture, {0,0,0,1}, helmetTransform);

    auto hoverTransform = glm::translate(glm::mat4(1.f), glm::vec3(6, 0, -14)) * glm::rotate(glm::mat4(1.f), deg2rad(90), glm::vec3(0,0,1)) * glm::scale(glm::mat4(1.f), glm::vec3(0.05f));
    hoverTransform = hoverTransform * glm::rotate(glm::mat4(1.f), deg2rad(90), glm::vec3(0,1,0));
    m_renderer->loadModel("Assets/soviet_trainer_spitfire/scene.gltf", programTexture, {0,0,0,1}, hoverTransform);

    m_mesh = new Plane({0,0,0,1});

    Mesh* cubeMesh = new Cube({0.1,0.1,0.8,1});
    auto cubeRo = new RenderObject(cubeMesh, m_materialTexture);
    auto cubeT =  glm::scale(glm::mat4(1.f), glm::vec3(0.8f)) * glm::translate(glm::mat4(1.f), glm::vec3(5, 2, -1)) * glm::rotate(glm::mat4(1.f), 90.f, glm::vec3(1,1,0)) ;
    cubeRo->setTransform(cubeT);
    m_renderer->addRenderObject(cubeRo);

    auto planeMesh = new Plane({0.855,0.855,0.855,1});
    auto planeRo = new RenderObject(planeMesh, m_material);
    auto planeT = glm::translate(glm::mat4(1.f), glm::vec3(0,-1.2f, 0)) * glm::rotate(glm::mat4(1.f), deg2rad(90.f), glm::vec3(-1,0,0)) * glm::scale(glm::mat4(1.f), glm::vec3(50.f));
    planeRo->setTransform(planeT);
    m_renderer->addRenderObject(planeRo);

    auto sphereMesh = new Sphere({0.1, 0.7, 0.1, 1.f});
    auto sphereRo = new RenderObject(sphereMesh, m_materialSpecular);
    auto sphereT = glm::translate(glm::mat4(1.f), glm::vec3(-4, 0, -3));
    sphereRo->setTransform(sphereT);
    m_renderer->addRenderObject(sphereRo);

    m_particle = new VerletParticle(sphereMesh, m_materialMicrofacet, glm::vec3(-8,10,-2), false);
    m_particle->addConstraint(Aabb(glm::vec3(-50, -0.2, -50), glm::vec3(50, 40, 50)));
    auto gravity = glm::vec3(0.0, -0.05, 0.0);
    m_particle->addBehavior(gravity);
    m_renderer->addRenderObject(m_particle);

    std::vector<std::vector<glm::vec3>> controlPoints = {
      {{-0.5f, -0.5f, 0.0f}, {0.f, 0.f, 0.0f}, {0.5f, 0.f, 0.0f}, {1.f, -0.5f, 0.0f}},
      {{-0.5f, -0.5f, 0.5f}, {0.f, 0.f, 0.5f}, {0.5f, 0.5f, 0.5f}, {1.f, -0.5f, 0.5f}},
      {{-0.5f,  -0.5f, 1.0f}, {0.f, 0.5f, 1.0f}, {0.83f, 1.3f, 1.0f}, {1.f, -0.3f, 1.0f}},
      {{-0.5f,  0.f, 1.5f}, {0.f, 0.f, 1.5f}, {0.5f, -1.0f, 1.5f}, {1.f, 0.3f, 1.5f}}
    };
    Mesh* mesh = new BezierSurface(controlPoints, 100, 100, glm::vec4(1.0f, 0.2f, 0.2f, 1.f));
    auto ro = new RenderObject(mesh, m_materialMicrofacet);
    auto bezierSurfaceT = glm::scale(glm::mat4(1.f), glm::vec3(3)) * glm::translate(glm::mat4(1.f), glm::vec3(-2, 0.5, -5));
    ro->setTransform(bezierSurfaceT);
    m_renderer->addRenderObject(ro);

    m_arm = new Cylinder({1,0,0,1}, 1, 1.0, 8.0, 64);
    for(auto& v : m_arm->m_vertices){
        m_arm->m_verticesInit.push_back(v);
    }

    m_cylinder = new Cylinder({1,0,0,1}, 1, 1.0, 8.0, 64);
    m_armRo = new RenderObject(m_arm, m_materialWeight);
    auto cylinderT = glm::translate(glm::mat4(1.f), glm::vec3(-11, 3.5, -8)) * glm::rotate(glm::mat4(1.f), deg2rad(90), glm::vec3(0,0,1));
    m_armRo->setTransform(cylinderT);
    m_renderer->addRenderObject(m_armRo);

    auto bone = new Bone({4,1.5,0}, {0,1.5,0}, m_materialBasic);
    bone->setPosition(glm::vec3(0));
    m_arm->m_boneList.push_back(bone);
    auto bone2 = new Bone({0,1.5,0}, {-4,1.5,0}, m_materialBasic);
    bone->m_childs.push_back(bone2);
    bone2->setPosition(glm::vec3(0,4,0));
    m_arm->m_boneList.push_back(bone2);
    m_arm->m_boneWeight.resize(m_arm->m_vertices.size());
    computeWeight();

    /*** Setup shadows ***/
    m_renderer->setupShadows(_camera->getNearPlane(), _camera->getFarPlane());

    // TODO: refactor into renderer
    m_material->getShader()->bind();
    m_material->getShader()->setUniform1i("shadowMap", 0);
    m_material->getShader()->setUniform1i("envMap", 1);
    m_material->getShader()->setUniform1f("farPlane", _camera->getFarPlane());
    auto& shadowCascadeLevels = m_renderer->getShadowCascadeLevels();
    m_material->getShader()->setUniform1i("cascadeCount", shadowCascadeLevels.size());
    for (size_t i = 0; i < shadowCascadeLevels.size(); ++i){
      m_material->getShader()->setUniform1f("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
    }  

    m_materialSpecular->getShader()->bind();
    m_materialSpecular->getShader()->setUniform1i("shadowMap", 0);
    m_materialSpecular->getShader()->setUniform1i("envMap", 1);
    m_materialSpecular->getShader()->setUniform1f("farPlane", _camera->getFarPlane());
    m_materialSpecular->getShader()->setUniform1i("cascadeCount", shadowCascadeLevels.size());
    for (size_t i = 0; i < shadowCascadeLevels.size(); ++i){
      m_materialSpecular->getShader()->setUniform1f("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
    }  

    m_materialLambert->getShader()->bind();
    m_materialLambert->getShader()->setUniform1i("shadowMap", 0);
    m_materialLambert->getShader()->setUniform1i("envMap", 1);
    m_materialLambert->getShader()->setUniform1f("farPlane", _camera->getFarPlane());
    m_materialLambert->getShader()->setUniform1i("cascadeCount", shadowCascadeLevels.size());
    for (size_t i = 0; i < shadowCascadeLevels.size(); ++i){
      m_materialLambert->getShader()->setUniform1f("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
    }  
}

BloomDemo::~BloomDemo() {
    for (unsigned int i = 0; i < m_mipChain.size(); i++) {
        glDeleteTextures(1, &m_mipChain[i].texture);
        m_mipChain[i].texture = 0;
    }
    delete m_fbo;
    delete m_mipfbo;
    delete m_renderer;
}

// Compute weights for cylinder
void BloomDemo::computeWeight(){
    float d =(float)(value_d)/200;
    float min = 0.5-d;
    float max = 0.5+d;
    std::vector<Vertex> newVertices;
    for(auto i=0; i< (int)m_cylinder->m_vertices.size(); i++){
        glm::vec2 tex = m_cylinder->m_vertices[i].m_texCoords;
        float a = tex.y;
        if(a<min) a = 0;
        else{
            if(a>max) a = 1;
            else{
                a = (a - min) / (max - min);
            }
        }
        m_arm->m_boneWeight[i].clear();
        m_arm->m_boneWeight[i].push_back(std::pair<int,float>(0,1-a));
        m_arm->m_boneWeight[i].push_back(std::pair<int,float>(1,a));

        m_arm->m_vertices[i].m_texCoords = glm::vec3(1-a,a,0);
        newVertices.push_back(m_arm->m_vertices[i]);
    }
    Mesh* newMesh = new Mesh(newVertices, m_arm->m_indices, GL_TRIANGLES);
    m_armRo->setMesh(newMesh);
}

// Compute rotations for cylinder
void BloomDemo::computeRotation() {
    m_arm->m_boneList[1]->rotateRestPose(-m_rot + 180.f, glm::vec3(0,0,1));
    m_arm->m_boneList[1]->rotate(-m_rot3 + 180.f, glm::vec3(0,1,0));
    m_arm->m_boneList[0]->rotateRestPose(-m_rot2 + 180, glm::vec3(0,0,1));
}

// Update mesh vertices with new weights
void BloomDemo::computeMesh() {
    std::vector<Vertex> newVertices;
    computeRotation();
    if(!m_animMode)
      return;
    for(unsigned int i=0; i < m_arm->m_vertices.size(); i++){
        glm::mat4 blend(0);
        for(unsigned int j=0; j < m_arm->m_boneWeight[i].size(); j++){
            std::pair<int,float> pair = m_arm->m_boneWeight[i][j];
            Bone* bone = m_arm->m_boneList[pair.first];
            blend += pair.second * bone->getBlendPose();
        }
        glm::vec4 initV = glm::vec4(m_arm->m_verticesInit[i].m_position,1);
        m_arm->m_vertices[i].m_position = blend * initV;
        newVertices.push_back(m_arm->m_vertices[i]);
    }
    Mesh* newMesh = new Mesh(newVertices, m_arm->m_indices, GL_TRIANGLES);
    m_armRo->setMesh(newMesh);
}

// Update scene with available parameters in GUI
void BloomDemo::compute() {
    m_programQuad->bind();
    m_programQuad->setUniform1f("exposure", m_exposure);
    m_programQuad->unbind();

    m_programTh->bind();
    m_programTh->setUniform1f("threshold", m_threshold);
    m_programTh->unbind();

    //m_currentRo->getMesh()->setColor(m_color);
    m_renderer->setDirLight(lightDir, glm::vec3(1));

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

    /*** Compute new camera transform ***/
    _view = _camera->viewmatrix();
    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, _camera->getNearPlane(), _camera->getFarPlane());

    // depth pass for shadow mapping
    m_renderer->setCameraPosition(_camera->position());
    m_renderer->setVP(_view, _projection);
    m_renderer->depthOnlyPass(_camera.get(), lightDir, _width, _height);

    // render scene as normal using the generated depth/shadow map  
    m_fbo->bind();
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    if (_drawfill)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glViewport(0, 0, _width, _height);

    /*** Update scene ***/
    m_renderer->setVP(_view, _projection);
    m_renderer->setCameraPosition(_camera->position());
    m_particle->update();

    auto pose = glm::mat4(1.f);
    m_materialWeight->getShader()->bind();
    if(m_animMode){
      m_materialWeight->getShader()->setMat4("bone1", pose);
      m_materialWeight->getShader()->setMat4("bone2", pose);
    }
    else {
      if(m_arm->m_boneList.size() >= 1){
        m_materialWeight->getShader()->setMat4("bone1", m_arm->m_boneList[0]->getBlendPose());
      }
      if(m_arm->m_boneList.size() >= 2){
        m_materialWeight->getShader()->setMat4("bone2", m_arm->m_boneList[1]->getBlendPose());
      }
    }
    m_materialWeight->getShader()->unbind();

    /*** Original render ***/
    m_renderer->draw();	

    glDrawBuffer(GL_COLOR_ATTACHMENT1);

    /*** Threshold original render ***/
    m_programTh->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    m_programTh->setUniform1f("threshold", m_threshold);
    m_renderer->draw(m_mesh, m_programTh);
    m_programTh->unbind();

    /*** Downscale texture ***/
    m_mipfbo->bind();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_DEPTH_TEST);

    m_programDown->bind();
    m_programDown->setUniform1f("srcResolutionX", (float)_width);
    m_programDown->setUniform1f("srcResolutionY", (float)_height);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fboThTexture);

    for(int i = 0; i < (int)m_mipChain.size(); i++){
        const BloomMip& mip = m_mipChain[i];
        glViewport(0, 0, mip.size.x, mip.size.y);

        m_mipfbo->attachTexture2D(GL_COLOR_ATTACHMENT0, mip.texture);

        // Render screen-filled quad of resolution of current mip
        m_renderer->draw(m_mesh, m_programDown);

        // Set current mip resolution as srcResolution for next iteration
        m_programDown->bind();
        m_programDown->setUniform1f("srcResolutionX", mip.size.x);
        m_programDown->setUniform1f("srcResolutionY", mip.size.y);
        glBindTexture(GL_TEXTURE_2D, mip.texture);
        m_programDown->unbind();
    }

    /*** Upscale texture ***/
    m_programUp->bind();
    m_programUp->setUniform1f("filterRadius", 0.005f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    for (int i = (int)m_mipChain.size() - 1; i > 0; i--)
    {
        const BloomMip& mip = m_mipChain[i];
        const BloomMip& nextMip = m_mipChain[i-1];

        // Bind viewport and texture from where to read
        m_programUp->bind();
        glBindTexture(GL_TEXTURE_2D, mip.texture);
        m_programUp->unbind();

        // Set framebuffer render target (we write to this texture)
        glViewport(0, 0, nextMip.size.x, nextMip.size.y);
        m_mipfbo->attachTexture(GL_COLOR_ATTACHMENT0, nextMip.texture);

        m_renderer->draw(m_mesh, m_programUp);
    }

    // Disable additive blending
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Restore if this was default
    glDisable(GL_BLEND);

    /*** final render to quad and gamma correction ***/
    m_mipfbo->unbind();
    glViewport(0,0, _width, _height);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.9f, 0.2f, 0.2f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    m_programQuad->bind();
    m_programQuad->setUniform1f("exposure", m_exposure);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_mipChain[0].texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    m_renderer->draw(m_mesh, m_programQuad);
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
        case 'm':
            m_animMode = !m_animMode;
            if(m_animMode){
              std::cout << "CPU side animation" << std::endl;
            }
            else
            {
              std::cout << "GPU side animation" << std::endl;
            }
            return true;
        default:
            return false;
    }
}
