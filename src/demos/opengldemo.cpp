
#define TINYOBJLOADER_IMPLEMENTATION
#include "opengldemo.h"
#include <iostream>

#include <QtImGui.h>
#include <implot.h>


OpenGLDemo::OpenGLDemo(int width, int height, ImVec4 clearColor) :  _width(width),
                       _height(height), m_clearColor(clearColor), _drawfill(true) {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);
}

OpenGLDemo::~OpenGLDemo() {
}

void OpenGLDemo::resize(int width, int height) {
   _width = width;
   _height = height;
    glViewport(0, 0, _width, _height);
}

void OpenGLDemo::draw() {
    glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    if (_drawfill)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

}

void OpenGLDemo::mouseclick(int , float , float ) {

}

void OpenGLDemo::mousemove(float , float ) {

}

void OpenGLDemo::mousewheel(float ) {

}

void OpenGLDemo::keyboardmove(int , double ) {

}

bool OpenGLDemo::keyboard(unsigned char ) {
    return false;
}

void OpenGLDemo::toggledrawmode() {
    _drawfill = !_drawfill;
}


// https://github.com/tinyobjloader/tinyobjloader
void readObjToTriangleMesh( const char* file,
                            tinyobj::attrib_t& attrib,
                            std::vector<tinyobj::shape_t>& shapes,
                            std::vector<tinyobj::material_t>& materials ) {
    std::string inputfile = file;
    std::string err, warn;

    bool ret =
        tinyobj::LoadObj( &attrib, &shapes, &materials, &warn,&err, inputfile.c_str(), "../assets/" );

    if ( !err.empty() ) { std::cout << err << std::endl; }

    if ( !ret ) exit( 1 );

    std::cout << "Model vertices: " << attrib.vertices.size() << std::endl;
}
