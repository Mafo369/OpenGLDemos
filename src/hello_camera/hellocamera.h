#ifndef SIMPLECAMERA_H
#define SIMPLECAMERA_H

#include "Renderer.h"

#include "camera.h"
#include "fwd.hpp"

#include <memory>
#include <functional>
#include <imgui.h>
#include <vector>


/** Simple drawing demonstration
 */
class SimpleCamera : public OpenGLDemo {
public:
    explicit SimpleCamera(int width, int height, ImVec4 clearColor);
    ~SimpleCamera() override;

    void resize(int width, int height) override;
    void draw() override;

    void mouseclick(int button, float xpos, float ypos) override;
    void mousemove(float xpos, float ypos) override;
    void mousewheel(float delta) override;

    void keyboardmove(int key, double time) override;
    bool keyboard(unsigned char k) override;

    std::vector<std::vector<glm::vec3>>& getControlsPoints() override { return m_controlPoints; }

    void compute() override;

private:
    // Shader program for rendering
    Shader* m_program;
    Shader* m_programNormal;
    Shader* m_programParametric;

    Renderer* m_renderer;

    bool m_first = true;

    std::vector<std::vector<glm::vec3>> m_controlPoints;

    // for mouse management
    int _button; // 0 --> left. 1 --> right. 2 --> middle. 3 --> other
    float _mousex{0};
    float _mousey{0};

    // Camera
    using CameraSelector=std::function<Camera*()>;
    std::vector<CameraSelector> _cameraselector;
    unsigned int _activecamera;

    std::unique_ptr<Camera> _camera;

    // matrices
    glm::mat4 _model;
    glm::mat4 _view;
    glm::mat4 _projection;
};

/*------------------------------------------------------------------------------------------------------------------------*/


#endif // SIMPLECAMERA_H
