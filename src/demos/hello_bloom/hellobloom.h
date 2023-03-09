#ifndef HELLOBLOOM_H
#define HELLOBLOOM_H

#include "../../Rendering/Renderer.h"

#include "../../Rendering/Texture.h"
#include "../../Rendering/Framebuffer.h"
#include "../camera.h"
#include "VerletParticle.h"
#include "Geometry/Cylinder.h"
#include "fwd.hpp"

#include <memory>
#include <functional>
#include <imgui.h>
#include <vector>

/** Simple drawing demonstration
 */
class BloomDemo : public OpenGLDemo {
public:
    explicit BloomDemo(int width, int height, ImVec4 clearColor);
    ~BloomDemo() override;

    void resize(int width, int height) override;
    void draw() override;

    void mouseclick(int button, float xpos, float ypos) override;
    void mousemove(float xpos, float ypos) override;
    void mousewheel(float delta) override;

    void keyboardmove(int key, double time) override;
    bool keyboard(unsigned char k) override;

    void compute() override;
    glm::vec4& getColor() override { return m_color; }
    void toggleCtrlPts() override { m_displayCtrlPts = !m_displayCtrlPts; }

    void computeRotation();
    void computeWeight();
    void computeMesh();

    float m_exposure = 1.f;
    float m_threshold = 0.935f;
    int value_d = 50;
    float rot = 180;
    float rot2 = 180;
    float rot3 = 180;

    glm::vec3 lightDir = glm::normalize(glm::vec3(20.0f, 50, 80.0f));
    std::shared_ptr<Material> m_materialMicrofacet;
    std::shared_ptr<Material> m_materialTexture;

    std::unique_ptr<Camera> _camera;

private:
    // Shader program for rendering
    std::shared_ptr<Material> m_material;
    std::shared_ptr<Material> m_materialSpecular;
    std::shared_ptr<Material> m_materialBasic;
    std::shared_ptr<Material> m_materialModified;
    std::shared_ptr<Material> m_materialLambert;
    std::shared_ptr<Material> m_materialNormal;
    std::shared_ptr<Material> m_materialParametric;
    std::shared_ptr<Material> m_materialQuad;
    std::shared_ptr<Material> m_materialTh;
    std::shared_ptr<Material> m_materialWeight;

    std::vector<BloomMip> m_mipChain;

    Framebuffer* m_fbo;
    Framebuffer* m_mipfbo;
    unsigned int m_fboTexture;
    unsigned int m_fboThTexture;
    unsigned int m_quadVAO;
    unsigned int m_quadVBO;
    unsigned int m_rbo;
    unsigned int m_attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    Shader* m_programQuad;
    Shader* m_programTh;
    Shader* m_programDown;
    Shader* m_programUp;

    Mesh* m_mesh;
    Shader* m_program;
    Renderer* m_renderer;
    RenderObject* m_currentRo;
    bool m_first = true;
    bool m_displayCtrlPts = false;

    // for mouse management
    int _button; // 0 --> left. 1 --> right. 2 --> middle. 3 --> other
    float _mousex{0};
    float _mousey{0};

    // Camera
    using CameraSelector=std::function<Camera*()>;
    std::vector<CameraSelector> _cameraselector;
    unsigned int _activecamera;

    // matrices
    glm::mat4 _lightmodel;
    glm::mat4 _view;
    glm::mat4 _projection;

    VerletParticle* m_particle;
    Cylinder* m_arm;
    Cylinder* m_cylinder;
    RenderObject* m_armRo;

    glm::vec4 m_color;

};

/*------------------------------------------------------------------------------------------------------------------------*/


#endif // HELLOBLOOM_H
