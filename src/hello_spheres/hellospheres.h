#ifndef SIMPLESPHERES_H
#define SIMPLESPHERES_H

#include <imgui.h>
#include "opengldemo.h"

#include "../hello_camera/camera.h"

#include <memory>

struct MathiasMesh;
/** Simple drawing demonstration
 */
class SimpleSpheres : public OpenGLDemo {
public:
    explicit SimpleSpheres(int width, int height, ImVec4 clearColor);
    ~SimpleSpheres();

    void resize(int width, int height) override;
    void draw() override;

    void mouseclick(int button, float xpos, float ypos) override;
    void mousemove(float xpos, float ypos) override;
    void mousewheel(float delta) override;

    void keyboardmove(int key, double time) override;
    bool keyboard(unsigned char k) override;
    void compute() override {}
    std::vector<std::vector<glm::vec3>>& getControlsPoints() override { return m_controlPoints; }

private:

    struct Sphere : public QOpenGLFunctions_4_1_Core {
        Sphere() {
          initializeOpenGLFunctions();
        }
        // A simple geometry
        std::vector<GLfloat> _vertices;
        std::vector<GLuint> _indices;

        // OpenGL object for geometry
        GLuint _vao;
        GLuint _vbo;
        GLuint _ebo;

        void buildFrom(const MathiasMesh &m);
        void draw();
    };
    std::vector<std::vector<glm::vec3>> m_controlPoints;
    Sphere _parametric;
    Sphere _geodesic;

    Sphere *_toDraw;

    // Shader program for rendering
    GLuint _program;
    int _mode;
    // for mouse management
    int _button; // 0 --> left. 1 --> right. 2 --> middle. 3 --> other
    float _mousex{0};
    float _mousey{0};

    // Camera
    std::unique_ptr<Camera> _camera;

    // matrices
    glm::mat4 _model;
    glm::mat4 _view;
    glm::mat4 _projection;
};

/*------------------------------------------------------------------------------------------------------------------------*/


#endif // SIMPLESPHERES_H
