#ifndef MYOPENGLWIDGET_H
#define MYOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_1_Core>
#include <QKeyEvent>

#include <memory>

#include <imgui.h>

#include "opengldemo.h"

#include <QTimer>


class MyOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core {

public:
    explicit MyOpenGLWidget(QWidget *parent = 0);

    ~MyOpenGLWidget();

    // size hints for the widget
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    // Demo management
    void activatedemo(unsigned int numdemo);

public slots:
    void cleanup();

protected:
    // OpenGL management
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    // Event maagement
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent( QWheelEvent* event ) override;

    void keyPressEvent(QKeyEvent *event) override;

private :
    std::unique_ptr<OpenGLDemo> _openglDemo;

    using DemoConstructors=std::function<OpenGLDemo*(int, int, ImVec4)>;
    std::vector<DemoConstructors> _democonstructors;

    // for event management
    std::int64_t _lastime;

    bool m_show_imgui_demo_window = true;
    bool m_show_implot_demo_window = false;
    ImVec4 m_clear_color = ImColor(114, 144, 154);
    glm::vec3 m_translation = glm::vec3(0.f);

    QTimer m_timer;
};

#endif // MYOPENGLWIDGET_H
