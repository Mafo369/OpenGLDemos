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

void addObjectsFromFile( const char* filename, std::vector<Vertex>& vector, std::vector<unsigned int>& indices, glm::vec4 color ) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    readObjToTriangleMesh( filename, attrib, shapes, materials );

    unsigned int curr_idx = 0;
    for ( size_t s = 0; s < shapes.size(); s++ ) {
        size_t index_offset = 0;
        for ( size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++ ) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            std::vector<glm::vec3> normals;
            std::vector<glm::vec2> texture;
            glm::vec2 textures[fv];

            // Loop over vertices in the face.
            for ( int v = 0; v < fv; v++ ) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx   = attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t vy   = attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t vz   = attrib.vertices[3 * idx.vertex_index + 2];
                glm::vec3 n;
                if ( !attrib.normals.empty() ) {
                    tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                    tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                    tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
                    n             = glm::vec3( nx, ny, nz );
                    normals.push_back( n );
                }
                if ( !attrib.texcoords.empty() ) {
                    tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                    tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];
                    textures[v]        = glm::vec2( tx, ty );
                }
                // Optional: vertex colors
                // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
                // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
                // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
                glm::vec3 v0 = glm::vec3( vx, vy, vz );
                Vertex vertex = {v0, n, textures[v], color};
                vector.push_back( vertex );
                indices.push_back(curr_idx++);
            }

            index_offset += fv;
        }
    }
}

//std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview)
//{
//    const auto inv = glm::inverse(projview);
//
//    std::vector<glm::vec4> frustumCorners;
//    for (unsigned int x = 0; x < 2; ++x)
//    {
//        for (unsigned int y = 0; y < 2; ++y)
//        {
//            for (unsigned int z = 0; z < 2; ++z)
//            {
//                const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
//                frustumCorners.push_back(pt / pt.w);
//            }
//        }
//    }
//
//    return frustumCorners;
//}
//
//std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
//{
//    return getFrustumCornersWorldSpace(proj * view);
//}
//
//glm::mat4 getLightSpaceMatrix(Camera* camera, const float nearPlane, const float farPlane, float width, float height, glm::vec3 lightDir)
//{
//    const auto proj = glm::perspective(
//        glm::radians(camera->zoom()), width / height, nearPlane,
//        farPlane);
//    const auto corners = getFrustumCornersWorldSpace(proj, camera->viewmatrix());
//
//    glm::vec3 center = glm::vec3(0, 0, 0);
//    for (const auto& v : corners)
//    {
//        center += glm::vec3(v);
//    }
//    center /= corners.size();
//
//    const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));
//
//    float minX = std::numeric_limits<float>::max();
//    float maxX = std::numeric_limits<float>::lowest();
//    float minY = std::numeric_limits<float>::max();
//    float maxY = std::numeric_limits<float>::lowest();
//    float minZ = std::numeric_limits<float>::max();
//    float maxZ = std::numeric_limits<float>::lowest();
//    for (const auto& v : corners)
//    {
//        const auto trf = lightView * v;
//        minX = std::min(minX, trf.x);
//        maxX = std::max(maxX, trf.x);
//        minY = std::min(minY, trf.y);
//        maxY = std::max(maxY, trf.y);
//        minZ = std::min(minZ, trf.z);
//        maxZ = std::max(maxZ, trf.z);
//    }
//
//    // Tune this parameter according to the scene
//    constexpr float zMult = 10.0f;
//    if (minZ < 0)
//    {
//        minZ *= zMult;
//    }
//    else
//    {
//        minZ /= zMult;
//    }
//    if (maxZ < 0)
//    {
//        maxZ /= zMult;
//    }
//    else
//    {
//        maxZ *= zMult;
//    }
//
//    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
//
//    return lightProjection * lightView;
//}
//
//std::vector<glm::mat4> getLightSpaceMatrices(Camera* camera, std::vector<float>& shadowCascadeLevels, float nearPlane, float farPlane, float width, float height, glm::vec3 lightDir)
//{
//    std::vector<glm::mat4> ret;
//    for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
//    {
//        if (i == 0)
//        {
//            ret.push_back(getLightSpaceMatrix(camera, nearPlane, shadowCascadeLevels[i], width, height, lightDir));
//        }
//        else if (i < shadowCascadeLevels.size())
//        {
//            ret.push_back(getLightSpaceMatrix(camera, shadowCascadeLevels[i - 1], shadowCascadeLevels[i], width, height, lightDir));
//        }
//        else
//        {
//            ret.push_back(getLightSpaceMatrix(camera, shadowCascadeLevels[i - 1], farPlane, width, height, lightDir));
//        }
//    }
//    return ret;
//}

BloomDemo::BloomDemo(int width, int height, ImVec4 clearColor) : OpenGLDemo(width, height, clearColor), _activecamera(0), _camera(nullptr) {
    /*** Initialise geometric data ***/
    m_color = glm::vec4(45.1f, 0.3f, 0.3f, 1.f);

    /*** Initialise renderer ***/
    m_renderer = new Renderer();

    /*** Create render objects ***/

    // Textures
    Texture* texture = new Texture("Assets/container2.png");
    Texture* textureSpecular = new Texture("Assets/container2_specular.png");
    
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

    m_renderer->setEnvMap("/home/mafo/dev/Raytracer/assets/Alexs_Apt_2k.hdr");

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
    matParams.texDiffuse = 0;
    matParams.texSpecular = 1;
    matParams.metallic = 0.6;
    matParams.roughness = 0.6;
    m_material = std::make_shared<Material>(program, matParams);
    m_materialSpecular = std::make_shared<Material>(programSpecular, matParams);
    m_materialMicrofacet = std::make_shared<Material>(programMicrofacet, matParams);
    m_materialBasic = std::make_shared<Material>(programBasic, matParams);
    m_materialModified = std::make_shared<Material>(programModified, matParams);
    m_materialLambert = std::make_shared<Material>(programLambert);
    m_materialNormal = std::make_shared<Material>(programNormal);
    m_materialParametric = std::make_shared<Material>(programParametric);
    m_materialTexture = std::make_shared<Material>(programTexture, matParams, texture, textureSpecular);
    
    m_first = false;

    /*** Create Camera ***/
    _cameraselector.push_back( []()->Camera*{return new EulerCamera(glm::vec3(0.f, 2.f, 7.f));} );
    _cameraselector.push_back( []()->Camera*{return new TrackballCamera(glm::vec3(0.f, 0.f, 1.f));} );
    _camera.reset(_cameraselector[_activecamera]());
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _view = _camera->viewmatrix();
    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, _camera->getNearPlane(), _camera->getFarPlane());

    /*** Create lights ***/
    LightParams lightParams;
    lightParams.position = glm::vec3(0.f, 13.f, 0.f);
    lightParams.color = glm::vec3(1.1f);
    Light* light = new Light(lightParams);
    
    lightParams.position = glm::vec3(1.f, 13.f, 0.f);
    lightParams.color = glm::vec3(0.1f);
    Light* light1 = new Light(lightParams);

    lightParams.position = glm::vec3(-1.f, 13.f, 0.0f);
    lightParams.color = glm::vec3(10.0f);
    Light* light2 = new Light(lightParams);

    /*** Attach lights to renderer ***/
    m_renderer->addLightRo(light);
    m_renderer->addLightRo(light1);
    m_renderer->addLightRo(light2);

    m_fbo = new Framebuffer();
    m_fbo->bind();
    
    int hdrWidth = _width;
    int hdrHeight = _height;

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

    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, hdrWidth, hdrHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
    m_fbo->attachRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    m_fbo->unbind();

    m_mipfbo = new Framebuffer();
    m_mipfbo->bind();

    glm::vec2 mipSize((float)hdrWidth, (float)hdrHeight);
    glm::ivec2 mipIntSize((int)hdrWidth, (int)hdrHeight);
    // Safety check
    if (hdrWidth > (int)INT_MAX || hdrHeight > (int)INT_MAX) {
        std::cerr << "Window size conversion overflow - cannot build bloom FBO!\n";
    }

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

    // attach it to currently bound framebuffer object
    m_mipfbo->attachTexture2D(GL_COLOR_ATTACHMENT0, m_mipChain[0].texture);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    m_mipfbo->unbind();

    std::vector<Vertex> verticesObj;
    std::vector<unsigned int> indicesObj;
    addObjectsFromFile("/home/mafo/dev/Raytracer/assets/Suzanne.obj", verticesObj, indicesObj, m_color);
    auto obj = new Mesh(verticesObj, indicesObj, GL_TRIANGLES);
    m_currentRo = new RenderObject(obj, m_materialBasic);
    auto monkeyTransform = glm::scale(glm::mat4(1.f), glm::vec3(0.48f));
    m_currentRo->setTransform(monkeyTransform);
    m_renderer->addRenderObject(m_currentRo);

    m_mesh = new Plane({0,0,0,1});

    Mesh* cubeMesh = new Cube({0.1,0.1,0.8,1});
    auto cubeRo = new RenderObject(cubeMesh, m_materialTexture);
    auto cubeT =  glm::scale(glm::mat4(1.f), glm::vec3(0.8f)) * glm::translate(glm::mat4(1.f), glm::vec3(3, 2, -1)) * glm::rotate(glm::mat4(1.f), 90.f, glm::vec3(1,1,0)) ;
    cubeRo->setTransform(cubeT);
    m_renderer->addRenderObject(cubeRo);

    auto planeMesh = new Plane({0.855,0.855,0.855,1});
    auto planeRo = new RenderObject(planeMesh, m_material);
    auto planeT = glm::translate(glm::mat4(1.f), glm::vec3(0,-1.2f, 0)) * glm::rotate(glm::mat4(1.f), deg2rad(90.f), glm::vec3(-1,0,0)) * glm::scale(glm::mat4(1.f), glm::vec3(50.f));
    planeRo->setTransform(planeT);
    m_renderer->addRenderObject(planeRo);

    auto sphereMesh = new Sphere({0.1, 0.7, 0.1, 1.f});
    auto sphereRo = new RenderObject(sphereMesh, m_materialSpecular);
    auto sphereT = glm::translate(glm::mat4(1.f), glm::vec3(-3, 0, -3));
    sphereRo->setTransform(sphereT);
    m_renderer->addRenderObject(sphereRo);

    std::vector<std::vector<glm::vec3>> controlPoints = {
      {{-0.5f, -0.5f, 0.0f}, {0.f, 0.f, 0.0f}, {0.5f, 0.f, 0.0f}, {1.f, -0.5f, 0.0f}},
      {{-0.5f, -0.5f, 0.5f}, {0.f, 0.f, 0.5f}, {0.5f, 0.5f, 0.5f}, {1.f, -0.5f, 0.5f}},
      {{-0.5f,  -0.5f, 1.0f}, {0.f, 0.5f, 1.0f}, {0.83f, 1.3f, 1.0f}, {1.f, -0.3f, 1.0f}},
      {{-0.5f,  0.f, 1.5f}, {0.f, 0.f, 1.5f}, {0.5f, -1.0f, 1.5f}, {1.f, 0.3f, 1.5f}}
    };
    Mesh* mesh = new BezierSurface(controlPoints, 100, 100, glm::vec4(1.0f, 0.5f, 0.2f, 1.f));
    RenderObject* ro = new RenderObject(mesh, m_materialMicrofacet);
    auto bezierSurfaceT = glm::scale(glm::mat4(1.f), glm::vec3(3)) * glm::translate(glm::mat4(1.f), glm::vec3(-2, 0.5, -5));
    ro->setTransform(bezierSurfaceT);
    m_renderer->addRenderObject(ro);

    m_renderer->setupShadows(_camera->getNearPlane(), _camera->getFarPlane());
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

void BloomDemo::compute() {
    m_programQuad->bind();
    m_programQuad->setUniform1f("exposure", m_exposure);
    m_programQuad->unbind();

    m_programTh->bind();
    m_programTh->setUniform1f("threshold", m_threshold);
    m_programTh->unbind();

    m_currentRo->getMesh()->setColor(m_color);
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
    m_material->getShader()->bind();
    m_material->getShader()->setUniform1i("shadowMap", 0);
    m_material->getShader()->setUniform1i("envMap", 1);
    m_material->getShader()->setUniform3f("viewPos", _camera->position());
    m_material->getShader()->setUniform3f("lightDir", lightDir);
    m_material->getShader()->setUniform1f("farPlane", _camera->getFarPlane());
    auto& shadowCascadeLevels = m_renderer->getShadowCascadeLevels();
    m_material->getShader()->setUniform1i("cascadeCount", shadowCascadeLevels.size());
    for (size_t i = 0; i < shadowCascadeLevels.size(); ++i){
      m_material->getShader()->setUniform1f("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
    }  
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_renderer->getLightDepthMaps());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderer->getCubeMap());

    m_materialSpecular->getShader()->bind();
    m_materialSpecular->getShader()->setUniform1i("shadowMap", 0);
    m_materialSpecular->getShader()->setUniform1i("envMap", 1);
    m_materialSpecular->getShader()->setUniform3f("viewPos", _camera->position());
    m_materialSpecular->getShader()->setUniform3f("lightDir", lightDir);
    m_materialSpecular->getShader()->setUniform1f("farPlane", _camera->getFarPlane());
    m_materialSpecular->getShader()->setUniform1i("cascadeCount", shadowCascadeLevels.size());
    for (size_t i = 0; i < shadowCascadeLevels.size(); ++i){
      m_materialSpecular->getShader()->setUniform1f("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
    }  
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_renderer->getLightDepthMaps());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderer->getCubeMap());

    /*** Update Material ***/
    m_renderer->setMaterialParams();
   
    /*** Update scene ***/
    m_renderer->setVP(_view, _projection);
    m_renderer->setCameraPosition(_camera->position());
    m_renderer->draw();	

    glDrawBuffer(GL_COLOR_ATTACHMENT1);

    m_programTh->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    m_programTh->setUniform1f("threshold", m_threshold);
    m_renderer->draw(m_mesh, m_programTh);
    m_programTh->unbind();

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
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Restore if this was default
    glDisable(GL_BLEND);

    // second pass
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
        case 'b' :
            m_renderer->setMaterial(m_materialBasic);
            return true;
        default:
            return false;
    }
}
