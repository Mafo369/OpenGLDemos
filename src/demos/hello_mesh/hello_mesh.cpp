#include "hello_mesh.h"
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

MeshDemo::MeshDemo(int width, int height, ImVec4 clearColor) : OpenGLDemo(width, height, clearColor), _activecamera(0), _camera(nullptr) {
    m_controlPoints.push_back({{0,0,0}});
    /*** Initialise geometric data ***/
    MyMesh mesh;
    std::vector<Vertex> vertices;
    mesh.request_vertex_normals();
    mesh.request_face_normals();

    if (!OpenMesh::IO::read_mesh(mesh, "/home/mafo/dev/Raytracer/assets/bunny.obj")) 
    {
      std::cerr << "read error\n";
      exit(1);
    }

    // If the file did not provide vertex normals, then calculate them
    OpenMesh::IO::Options opt;
    if ( !opt.check( OpenMesh::IO::Options::VertexNormal ) &&
         mesh.has_face_normals() && mesh.has_vertex_normals() )
    {
      // let the mesh update the normals
      mesh.update_normals();
    }

    MyMesh::VertexIter v_it, v_end(mesh.vertices_end());
    for(v_it=mesh.vertices_begin(); v_it != v_end; ++v_it) {
        MyMesh::Point p = mesh.point(*v_it);
        MyMesh::Normal n = mesh.normal(*v_it);
        std::cout << "Vertex #" << *v_it << ": " << p << std::endl;
        std::cout << "Normal #" << *v_it << ": " << n << std::endl;
        Vertex v;
        v.m_position = glm::vec3(p[0], p[1], p[2]);
        v.m_normal = glm::vec3(n[0], n[1], n[2]);
        v.m_color = m_color;
        vertices.push_back(v);
    }
    std::cout << "END" << std::endl;
    MyMesh::FaceIter f_it, f_end(mesh.faces_end());
    for(f_it = mesh.faces_begin(); f_it != f_end; ++f_it){
      for(auto v_it=f_it->vertices().begin(); v_it != f_it->vertices().end(); ++v_it) {
          m_indices.push_back(v_it->idx());
      }
    }

    auto cube = new Mesh(vertices, m_indices, GL_TRIANGLES);

    m_myMesh = mesh;

    /*** Initialise renderer ***/
    m_renderer = new Renderer();

    /*** Create render objects ***/
    
    // Shaders
    Shader* program = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/Microfacet.frag.glsl");
    Shader* programModified = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/MicrofacetModified.frag.glsl");
    Shader* programLambert = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/Lambert.frag.glsl");
    Shader* programNormal = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/Normals.frag.glsl");

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
    
    m_currentMaterial = m_material;

    auto ro = new RenderObject(cube, m_material);
    m_renderer->addRenderObject(ro);
    
    // Render objects
    m_first = false;

    /*** Create Camera ***/
    _cameraselector.push_back( []()->Camera*{return new EulerCamera(glm::vec3(0.f, 0.f, 3.f));} );
    _cameraselector.push_back( []()->Camera*{return new TrackballCamera(glm::vec3(0.f, 0.f, 3.f));} );
    _camera.reset(_cameraselector[_activecamera]());
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _view = _camera->viewmatrix();
    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, 0.1f, 100.0f);
    _model = glm::translate(glm::mat4(1.0), m_objTranslation);

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

MeshDemo::~MeshDemo() {
    delete m_renderer;
    for(auto& l : m_lights)
        delete l;
}

void MeshDemo::compute(bool update) {
  // (linearly) iterate over all vertices
  std::vector<Vertex> vertices;
  std::vector<MyMesh::Point> new_vertices;
  for (MyMesh::VertexIter v_it=m_myMesh.vertices_sbegin(); v_it!=m_myMesh.vertices_end(); ++v_it)
  {
    // circulate around the current vertex
    int N = 0;
    OpenMesh::Vec3f sum = {0.f, 0.f, 0.f};
    for (MyMesh::VertexVertexIter vv_it=m_myMesh.vv_iter(*v_it); vv_it.is_valid(); ++vv_it)
    {
      sum += m_myMesh.point(*vv_it);
      N++;
    }
    auto ci = (1.f / N) * sum;
    float alpha = 0.75;
    MyMesh::Point p;
    if(update){
       p = alpha * m_myMesh.point(*v_it) + (1.f - alpha) * ci;
    }
    else
    {
      p = m_myMesh.point(*v_it);
    }
    MyMesh::Normal n = m_myMesh.normal(*v_it);
    Vertex v;
    v.m_position = glm::vec3(p[0], p[1], p[2]);
    v.m_normal = glm::vec3(n[0], n[1], n[2]);
    v.m_color = m_color;
    vertices.push_back(v);
    new_vertices.push_back(p);
  }

  Mesh* mesh = new Mesh(vertices, m_indices, GL_TRIANGLES);
  RenderObject* ro;
  ro = new RenderObject(mesh, m_material);

  /*** Delete old render objects and add new ones ***/
  m_renderer->clearRenderObjects();
  m_renderer->addRenderObject(ro);

  std::vector<MyMesh::Point>::iterator p_iter;
  MyMesh::VertexIter v_iter;

  for(v_iter = m_myMesh.vertices().begin(), p_iter = new_vertices.begin();
      v_iter != m_myMesh.vertices().end();
      v_iter++, p_iter++)
  {
    m_myMesh.set_point(*v_iter, *p_iter);
  }
}

void MeshDemo::resize(int width, int height){
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

void MeshDemo::draw() {
    OpenGLDemo::draw();

    /*** Compute new camera transform ***/
    //_model = glm::translate(glm::mat4(1.0), m_translation);
    _view = _camera->viewmatrix();
    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, 0.1f, 100.0f);

    /*** Update Material ***/
    m_currentMaterial = m_renderer->getCurrentMaterial();
    m_renderer->setMaterialParams();
   
    /*** Update scene ***/
    _model = glm::translate(glm::mat4(1.f), m_objTranslation) * glm::rotate(glm::mat4(1.f), deg2rad(m_rotation), glm::vec3(1,0,0)) * glm::scale(glm::mat4(1.f), glm::vec3(m_scale));
    m_renderer->setMVP(_model, _view, _projection);
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

void MeshDemo::mouseclick(int button, float xpos, float ypos) {
    _button = button;
    _mousex = xpos;
    _mousey = ypos;
    _camera->processmouseclick(_button, xpos, ypos);
}

void MeshDemo::mousemove(float xpos, float ypos) {
    _camera->processmousemovement(_button, xpos, ypos, true);
}

void MeshDemo::mousewheel(float delta) {
    _camera->processmousescroll(delta);
}

void MeshDemo::keyboardmove(int key, double time) {
    _camera->processkeyboard(Camera_Movement(key), time);
}

bool MeshDemo::keyboard(unsigned char k) {
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
        case 'l' :
            m_renderer->setMaterial(m_materialLambert);
            return true;
        default:
            return false;
    }
}
