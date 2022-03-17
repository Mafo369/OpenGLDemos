#include "hellocamera.h"
#include <iostream>
#include <math.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/


#define deg2rad(x) float(M_PI)*(x)/180.f

void pascalCoeffs(int n, std::vector<std::vector<int>>& coeffs){
    for(int line = 1; line <= n; line++){
        int C = 1;
        std::vector<int> coeffI;
        for(int i = 1; i <= line; i++){
            coeffI.push_back(C);
            C = C * (line - i) / i;
        }
        coeffs.push_back(coeffI);
    }
}

SimpleCamera::SimpleCamera(int width, int height) : OpenGLDemo(width, height), _activecamera(0), _camera(nullptr) {
    // Initialise geometric data
    std::vector<std::vector<glm::vec3>> points;
    std::vector<glm::vec3> points1;
    std::vector<glm::vec3> points2;
    points1.push_back(glm::vec3(0.5f,  0.5f, 0.0f));
    points1.push_back(glm::vec3(0.5f, -0.5f, 0.0f));
    points1.push_back(glm::vec3(-0.5f, -0.5f, 0.0f));
    points1.push_back(glm::vec3(-0.5f, 0.5f, 0.0f));
    points2.push_back(glm::vec3(0.5f,  0.5f, 0.0f));
    points2.push_back(glm::vec3(0.5f, -0.5f, 0.0f));
    points2.push_back(glm::vec3(-0.5f, -0.5f, 0.0f));
    points2.push_back(glm::vec3(-0.5f, 0.5f, 0.0f));
    points.push_back(points1);
    points.push_back(points2);

    //std::vector<glm::vec3> points;
    //points.push_back(glm::vec3(0.5f,  0.5f, 0.0f));
    //points.push_back(glm::vec3(0.5f, -0.5f, 0.0f));
    //points.push_back(glm::vec3(-0.5f, -0.5f, 0.0f));
    //points.push_back(glm::vec3(-0.5f, 0.5f, 0.0f));
    Vertex v0 = {points1[0], glm::vec3(0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v1 = {points1[1], glm::vec3(0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v2 = {points1[2], glm::vec3(-0.577350269189626f, -0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    Vertex v3 = {points1[3], glm::vec3(-0.577350269189626f, 0.577350269189626f, 0.577350269189626f), glm::vec2(0.f, 0.f)};
    std::vector<Vertex> vertices = { v0, v1, v2, v3 };
    std::vector<unsigned int> indices = {
        0, 1, 1,   // First Triangle
        2, 2, 3    // Second Triangle
    };
    std::vector<Texture> textures;

    int n = points.size();
    int m = points1.size();
    std::vector<std::vector<int>> coeffsn;
    std::vector<std::vector<int>> coeffsm;
    pascalCoeffs(n, coeffsn);
    pascalCoeffs(m, coeffsm);

    //std::vector<Vertex> vertices;
    //std::vector<unsigned int> indices;
    auto normal = glm::vec3(0.577350269189626f, 0.577350269189626f, 0.0);
    for(int ui = 0; ui <= 10; ui++){
        float u = float(ui)/10.f;
        for(int uj = 0; uj <= 100; uj++){
            float v = float(uj)/100.f;
            glm::vec3 puv(0.0f);
            for(int i = 0; i < n; i++){
                for(int j = 0; j < m; j++){
                    float Bi = coeffsn[n-1][i] * powf(u, i) * powf(1.0f-u, n-1-i);  
                    float Bj = coeffsm[m-1][j] * powf(v, j) * powf(1.0f-v, m-1-j);  
                    puv += Bi * Bj * points[i][j];
                }
            }
            std::cout << puv.x << " " <<puv.y << " " <<puv.z << " " << std::endl;
            Vertex vertex = { normalize(puv), normal, glm::vec2(0.f, 0.f) };
            vertices.push_back(vertex);
        }
    }
    for(int i = 4; i < 104; i++){
        indices.push_back(10*i);
        indices.push_back(10*(i+1));
    }

    //int indice = 4;
    //std::vector<Vertex> vertices;
    //std::vector<unsigned int> indices;
    //auto normal = glm::vec3(0.577350269189626f, 0.0, 0.0);
    //for(int j = 0; j <= 100; j++){
    //    float u = float(j)/100.f;
    //    glm::vec3 pu(0.0f);
    //    for(int i = 0; i < n; i++){
    //        float Bi = coeffs[n-1][i] * powf(u, i) * powf(1.0f-u, n-1-i);  
    //        pu += Bi * points[i];
    //    }
    //    indices.push_back(indice);
    //    indice++;
    //    indices.push_back(indice);
    //    Vertex v = { pu, normal, glm::vec2(0.f, 0.f) };
    //    vertices.push_back(v);
    //}
    //indices.pop_back();
 
    m_renderer = new Renderer();

    m_mesh = new Mesh(vertices, indices, textures, GL_LINES);

    m_program = 
        new Shader("/home/mafo/dev/helloOpenGL/Shaders/Camera.vert.glsl", "/home/mafo/dev/helloOpenGL/Shaders/Camera.frag.glsl");


    _cameraselector.push_back( []()->Camera*{return new EulerCamera(glm::vec3(0.f, 0.f, 3.f));} );
    _cameraselector.push_back( []()->Camera*{return new TrackballCamera(glm::vec3(0.f, 0.f, 3.f));} );

    _camera.reset(_cameraselector[_activecamera]());

    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _view = _camera->viewmatrix();

    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, 0.1f, 100.0f);

    _model = glm::mat4(1.0);
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

    _view = _camera->viewmatrix();
    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, 0.1f, 100.0f);

    m_program->setMVP(_model, _view, _projection);
    m_renderer->draw(m_mesh, m_program);
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
