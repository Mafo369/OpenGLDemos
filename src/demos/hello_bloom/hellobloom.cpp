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

void addObjectsFromFile( const char* filename, std::vector<Vertex>& vector, std::vector<unsigned int>& indices ) {
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
                Vertex vertex = {v0, n, textures[v], {1,0,0,1}};
                vector.push_back( vertex );
                indices.push_back(curr_idx++);
            }

            index_offset += fv;

            //glm::vec3 v0 = vector[0];
            //glm::vec3 v1 = vector[1];
            //glm::vec3 v2 = vector[2];
            //glm::vec3 n;
            //if ( normals.empty() ) {
            //    glm::vec3 v2v0 = v2 - v0;
            //    glm::vec3 v1v0 = v1 - v0;
            //    n         = normalize( cross( v1v0, v2v0 ) );
            //    normals   = { n, n, n };
            //}
            //else {
            //    n = normalize( normals[0] + normals[1] + normals[2] );
            //}

            //if ( !materials.empty() ) {
            //    // do something
            //}
            //else {
            //    // do something
            //}
        }
    }
}

BloomDemo::BloomDemo(int width, int height, ImVec4 clearColor) : OpenGLDemo(width, height, clearColor), _activecamera(0), _camera(nullptr) {
    /*** Initialise geometric data ***/
    m_color = glm::vec4(120.0f, 0.1f, 0.1f, 1.f);

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
    Shader* programBasic = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/Basic.frag.glsl");

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
    m_programTh->setUniform1i("threshold", m_threshold);
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
    m_materialBasic = std::make_shared<Material>(programBasic, matParams);
    m_materialModified = std::make_shared<Material>(programModified, matParams);
    m_materialLambert = std::make_shared<Material>(programLambert);
    m_materialNormal = std::make_shared<Material>(programNormal);
    m_materialParametric = std::make_shared<Material>(programParametric);
    m_materialTexture = std::make_shared<Material>(programTexture, matParams, texture, textureSpecular);
    
    m_currentMaterial = m_material;
    m_currentBloomShader = m_programQuad;
    
    // Render objects
    //compute();
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
    m_lights.push_back(light);
    m_lights.push_back(light1);
    m_lights.push_back(light2);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    //stbi_set_flip_vertically_on_load(true);
    int hdrWidth, hdrHeight, nrComponents;
    //float *data = stbi_loadf("/home/mafo/dev/Raytracer/assets/rainforest_trail_4k.hdr", &hdrWidth, &hdrHeight, &nrComponents, 0);
    //if (!data)
    //{
    //    std::cout << "Failed to load HDR image." << std::endl;
    //}  
    
    hdrWidth = _width;
    hdrHeight = _height;

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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTexture, 0);

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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_fboThTexture, 0);

    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, hdrWidth, hdrHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  

    glGenFramebuffers(1, &m_mipfbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_mipfbo);

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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_mipChain[0].texture, 0);

    //glDrawBuffers(1, m_attachments);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  

    m_currentFbo = m_fboTexture;

    Vertex v0 = {glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec3(0), glm::vec2(0.f, 1.f), m_color};
    Vertex v1 = {glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0), glm::vec2(0.f, 0.f), m_color};
    Vertex v2 = {glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0), glm::vec2(1.f, 0.f), m_color};
    Vertex v3 = {glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0), glm::vec2(0.f, 1.f), m_color};
    Vertex v4 = {glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0), glm::vec2(1.f, 0.f), m_color};
    Vertex v5 = {glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0), glm::vec2(1.f, 1.f), m_color};
    std::vector<Vertex> vertices = { v0, v1, v2, v3, v4, v5 };
    std::vector<unsigned int> indices = {
        0, 1, 2,   // First Triangle
        3, 4, 5    // Second Triangle
    };
    

    std::vector<Vertex> verticesObj;
    std::vector<unsigned int> indicesObj;

    addObjectsFromFile("/home/mafo/Downloads/uploads_files_3783485_OM.obj", verticesObj, indicesObj);

    auto obj = new Mesh(verticesObj, indicesObj, GL_TRIANGLES);
    m_currentRo = new RenderObject(obj, m_material);
    m_renderer->addRenderObject(m_currentRo);
 
    m_mesh = new Mesh(vertices, indices, GL_TRIANGLES);

    glm::vec4 planeColor = {0.5,0.5,0.5,1};
    Vertex v0P = {glm::vec3(-5., -1.2f,  5.), glm::vec3(0,1,0), glm::vec2(0.f, 1.f), planeColor};
    Vertex v1P = {glm::vec3(-5., -1.2f, -5.), glm::vec3(0,1,0), glm::vec2(0.f, 0.f), planeColor};
    Vertex v2P = {glm::vec3(5., -1.2f, -5.), glm::vec3(0,1,0), glm::vec2(1.f, 0.f), planeColor};
    Vertex v3P = {glm::vec3(-5., -1.2f, 5.), glm::vec3(0,1,0), glm::vec2(0.f, 1.f), planeColor};
    Vertex v4P = {glm::vec3(5., -1.2f, -5.), glm::vec3(0,1,0), glm::vec2(1.f, 0.f), planeColor};
    Vertex v5P = {glm::vec3(5., -1.2f, 5.), glm::vec3(0,1,0), glm::vec2(1.f, 1.f), planeColor};
    std::vector<Vertex> verticesPlane = { v0P, v1P, v2P, v3P, v4P, v5P };
    std::vector<unsigned int> indicesPlane = {
        0, 1, 2,   // First Triangle
        3, 4, 5    // Second Triangle
    };
    auto planeMesh = new Mesh(verticesPlane, indicesPlane, GL_TRIANGLES);
    auto planeRo = new RenderObject(planeMesh, m_material);
    m_renderer->addRenderObject(planeRo);
}

BloomDemo::~BloomDemo() {
    for (unsigned int i = 0; i < m_mipChain.size(); i++) {
        glDeleteTextures(1, &m_mipChain[i].texture);
        m_mipChain[i].texture = 0;
    }
    glDeleteFramebuffers(1, &m_fbo);
    m_fbo = 0;
    delete m_renderer;
    for(auto& l : m_lights)
        delete l;
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
    _model = glm::scale(glm::mat4(1.f), glm::vec3(0.28f));
    _model = _model * glm::translate(glm::mat4(1.0), m_translation);
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

    // first pass
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glDrawBuffers(2, m_attachments);
    glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    glEnable(GL_DEPTH_TEST);
    if (_drawfill)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    m_renderer->draw();	

    //glDrawBuffer(GL_COLOR_ATTACHMENT1);

    m_programTh->bind();
    //glActiveTexture(GL_TEXTURE0);
    m_programTh->setUniform1f("threshold", m_threshold);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    m_renderer->draw(m_mesh, m_programTh);
    m_programTh->unbind();

    glBindFramebuffer(GL_FRAMEBUFFER, m_mipfbo);
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

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, mip.texture, 0);

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
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, nextMip.texture, 0);

        m_renderer->draw(m_mesh, m_programUp);
    }

    // Disable additive blending
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Restore if this was default
    glDisable(GL_BLEND);

    // second pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glViewport(0,0, _width, _height);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.9f, 0.2f, 0.2f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_programQuad->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_currentFbo);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    m_programQuad->unbind();
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
        case 'a' :
            if(m_currentFbo == m_fboTexture)
              m_currentFbo = m_mipChain[0].texture;
            else
              m_currentFbo = m_fboTexture;
            return true;
        case 'h' :
            m_currentBloomShader = m_programTh;
            return true;
        case 'q' :
            m_currentBloomShader = m_programQuad;
            return true;
        default:
            return false;
    }
}
