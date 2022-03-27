#ifndef SCENE_H
#define SCENE_H


#include <vector>

#include "error_handling.h"
#include <QOpenGLFunctions_4_1_Core>
#include <imgui.h>

/** Simple class for managing an OpenGL demo
 */
class OpenGLDemo : public QOpenGLFunctions_4_1_Core {

public:
    explicit OpenGLDemo(int width, int height, ImVec4 clearColor);
    virtual ~OpenGLDemo();

    virtual void resize(int width, int height);
    virtual void draw();

    virtual void mouseclick(int button, float xpos, float ypos);
    virtual void mousemove(float xpos, float ypos);
    virtual void mousewheel(float delta);
    virtual void keyboardmove(int key, double time);
    virtual bool keyboard(unsigned char k);

    void setClearColor(ImVec4 clearColor) { m_clearColor = clearColor; }
    void setTranslation(glm::vec3 translation) { m_translation = translation; }
    virtual std::vector<std::vector<glm::vec3>>& getControlsPoints() { return m_controlPoints; }
    virtual void compute() {}
    virtual glm::vec4& getColor() { return m_color; }

    void toggledrawmode();

    virtual void toggleCtrlPts() {}

protected:
    // Width and heigth of the viewport
    int _width;
    int _height;
    ImVec4 m_clearColor;
    glm::vec3 m_translation = glm::vec3(0.f);
    glm::vec4 m_color;
    std::vector<std::vector<glm::vec3>> m_controlPoints;

private:
    // Rendering mode (true is filled, false is wireframed
    bool _drawfill;
};


#endif // SCENE_H
