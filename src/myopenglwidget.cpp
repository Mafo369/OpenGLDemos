#include "myopenglwidget.h"

#include <QMessageBox>
#include <QApplication>
#include <QDateTime>

#include <iostream>
#include <stdexcept>

#include <QtImGui.h>
#include <implot.h>
#include <imgui.h>
#include <QTimer>
#include <string>

#include "hello_triangles/hellotriangles.h"
#include "hello_camera/hellocamera.h"
#include "hello_spheres/hellospheres.h"

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) :QOpenGLWidget(parent)/*, QOpenGLFunctions_4_1_Core()*/, _openglDemo(nullptr), _lastime(0) {
    // add all demo constructors here
    _democonstructors.push_back( [](int width, int height, ImVec4 clearColor)->OpenGLDemo*{
        std::cout << "Hello clear ..." << std::endl; return new OpenGLDemo(width, height, clearColor);
        } );
    _democonstructors.push_back( [](int width, int height, ImVec4 clearColor)->OpenGLDemo*{
        std::cout << "Hello triangles ..." << std::endl; return new SimpleTriangle(width, height, clearColor);
        } );
    _democonstructors.push_back( [](int width, int height, ImVec4 clearColor)->OpenGLDemo*{
        std::cout << "Hello camera ..." << std::endl; return new SimpleCamera(width, height, clearColor);
        } );
    _democonstructors.push_back( [](int width, int height, ImVec4 clearColor)->OpenGLDemo*{
        std::cout << "Hello spheres ..." << std::endl; return new SimpleSpheres(width, height, clearColor);
        } );
}

MyOpenGLWidget::~MyOpenGLWidget() {

}

QSize MyOpenGLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize MyOpenGLWidget::sizeHint() const
{
    return QSize(512, 512);
}

void MyOpenGLWidget::cleanup() {
    _openglDemo.reset(nullptr);
}

void MyOpenGLWidget::initializeGL() {
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &MyOpenGLWidget::cleanup);

    QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer.start(16);

    if (!initializeOpenGLFunctions()) {
        QMessageBox::critical(this, "OpenGL initialization error", "MyOpenGLWidget::initializeGL() : Unable to initialize OpenGL functions");
        exit(1);
    }
    // Initialize OpenGL and all OpenGL dependent stuff below
    QtImGui::initialize(this);
    _openglDemo.reset(_democonstructors[0](width(), height(), m_clear_color));
}

void MyOpenGLWidget::paintGL() {
    QtImGui::newFrame();
    {
        ImGui::Text("Hello, world!");
        ImGui::ColorEdit3("clear color", (float*)&m_clear_color);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::SliderFloat3("Translation", glm::value_ptr(m_translation), -5.f, 5.0);
        if(!_openglDemo->getControlsPoints().empty()){
            ImGui::Text("Control points:");
            auto& ctrlPts = _openglDemo->getControlsPoints();
            for(unsigned int i = 0; i < ctrlPts.size(); i++){
                for(unsigned int j = 0; j < ctrlPts[0].size(); j++){
                    std::string name = "ControlPoint " + std::to_string(i) + " " + std::to_string(j);
                    if(ImGui::SliderFloat3(name.c_str(), glm::value_ptr(ctrlPts[i][j]), -5.f, 5.0)){
                        _openglDemo->compute();
                    }
                }
            }
            //auto ctrlPts = _openglDemo->getControlsPoints();
            //for(unsigned int i = 0; ctrlPts.size(); i++){
            //    for(unsigned int j = 0; ctrlPts[i].size(); j++){
            //        std::string name = "ControlPoint " + std::to_string(i);
            //        ImGui::SliderFloat3(name.c_str(), glm::value_ptr(ctrlPts[i][j]), -5.f, 5.0);
            //    }
            //    //_openglDemo->compute(); 
            //}
        }
    }

    std::int64_t starttime = QDateTime::currentMSecsSinceEpoch();
    _openglDemo->setClearColor(m_clear_color);
    _openglDemo->setTranslation(m_translation);
    _openglDemo->draw();
    glFinish();
    std::int64_t endtime = QDateTime::currentMSecsSinceEpoch();
    _lastime = endtime-starttime;

    ImGui::Render();
    QtImGui::render();
}

void MyOpenGLWidget::resizeGL(int width, int height) {
    _openglDemo->resize(width, height);
}

void MyOpenGLWidget::mousePressEvent(QMouseEvent *event) {
    // buttons are 0(left), 1(right) to 2(middle)
    int b;
    Qt::MouseButton button=event->button();
    if (button & Qt::LeftButton) {
        if ((event->modifiers() & Qt::AltModifier))
            b = 2;
        else
            b = 0;
    } else if (button & Qt::RightButton)
        b = 1;
    else if (button & Qt::MiddleButton)
        b = 2;
    else
        b=3;
    _openglDemo->mouseclick(b, event->x(), event->y());
    _lastime = QDateTime::currentMSecsSinceEpoch();
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
    if(!ImGui::GetIO().WantCaptureMouse){
        _openglDemo->mousemove(event->x(), event->y());
        update();
    }
}

void MyOpenGLWidget::wheelEvent( QWheelEvent* event ) {
    if(!ImGui::GetIO().WantCaptureMouse){
        _openglDemo->mousewheel( event->angleDelta().y() * 0.01f + event->angleDelta().x() * 0.01f );
        update();
    }
}

void MyOpenGLWidget::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
        // Demo keys
        case Qt::Key_0:
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        case Qt::Key_6:
        case Qt::Key_7:
        case Qt::Key_8:
        case Qt::Key_9:
            activatedemo(event->key()-Qt::Key_0);
        break;
        // Move keys
        case Qt::Key_Left:
        case Qt::Key_Up:
        case Qt::Key_Right:
        case Qt::Key_Down:
            if(!ImGui::GetIO().WantCaptureMouse){
                _openglDemo->keyboardmove(event->key()-Qt::Key_Left, 1./100/*double(_lastime)/10.*/);
                update();
            }
        break;
        // Wireframe key
        case Qt::Key_W:
            if(!ImGui::GetIO().WantCaptureMouse){
                _openglDemo->toggledrawmode();
                update();
            }
        break;
        // Other keys are transmitted to the scene
        default :
            if(!ImGui::GetIO().WantCaptureMouse){
                if (_openglDemo->keyboard(event->text().toStdString()[0]))
                    update();
            }
        break;
    }
}

void MyOpenGLWidget::activatedemo(unsigned int numdemo) {
    if (numdemo < _democonstructors.size()) {
        std::cout << "Activating demo " << numdemo << " : ";
        makeCurrent();
        _openglDemo.reset(_democonstructors[numdemo](width(), height(), m_clear_color));
        doneCurrent();
        update();
    }
}

