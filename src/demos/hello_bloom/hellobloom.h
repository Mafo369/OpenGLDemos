#ifndef HELLOBLOOM_H
#define HELLOBLOOM_H

#include "../../Rendering/Renderer.h"

#include "../../Rendering/Texture.h"
#include "../camera.h"
#include "fwd.hpp"

#include <memory>
#include <functional>
#include <imgui.h>
#include <vector>

typedef struct s_BloomMip
{
    glm::vec2 size;
    glm::ivec2 intSize;
    unsigned int texture;
}BloomMip;


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

    float m_exposure = 0.358f;
    float m_threshold = 0.717f;

private:
    // Shader program for rendering
    std::shared_ptr<Material> m_material;
    std::shared_ptr<Material> m_materialBasic;
    std::shared_ptr<Material> m_materialModified;
    std::shared_ptr<Material> m_materialLambert;
    std::shared_ptr<Material> m_materialNormal;
    std::shared_ptr<Material> m_materialParametric;
    std::shared_ptr<Material> m_materialTexture;
    std::shared_ptr<Material> m_materialQuad;
    std::shared_ptr<Material> m_materialTh;

    std::vector<BloomMip> m_mipChain;

    unsigned int m_fbo;
    unsigned int m_mipfbo;
    unsigned int m_fboTexture;
    unsigned int m_fboThTexture;
    unsigned int m_quadVAO;
    unsigned int m_quadVBO;
    unsigned int m_rbo;
    unsigned int m_attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    unsigned int m_CMTexture;

    unsigned int captureFBO, captureRBO;
    unsigned int envCubemap;

    Shader* m_programQuad;
    Shader* m_programTh;
    Shader* m_programDown;
    Shader* m_programUp;
    Shader* m_programCube;
    Shader* m_programBg;

    Mesh* m_mesh;
    Mesh* m_cubeMesh;
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

    std::unique_ptr<Camera> _camera;

    // matrices
    glm::mat4 _model;
    glm::mat4 _lightmodel;
    glm::mat4 _view;
    glm::mat4 _projection;

    glm::vec4 m_color;
};

/*------------------------------------------------------------------------------------------------------------------------*/


#endif // HELLOBLOOM_H
