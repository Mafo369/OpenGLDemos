#ifndef HELLO_MESH_H
#define HELLO_MESH_H

#include "../../Rendering/Renderer.h"

#include "../../Rendering/Texture.h"
#include "../camera.h"
#include "fwd.hpp"

#include <memory>
#include <functional>
#include <imgui.h>
#include <vector>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
typedef OpenMesh::TriMesh_ArrayKernelT<>  MyMesh;


/** Simple drawing demonstration
 */
class MeshDemo : public OpenGLDemo {
public:
    explicit MeshDemo(int width, int height, ImVec4 clearColor);
    ~MeshDemo() override;

    void resize(int width, int height) override;
    void draw() override;

    void mouseclick(int button, float xpos, float ypos) override;
    void mousemove(float xpos, float ypos) override;
    void mousewheel(float delta) override;

    void keyboardmove(int key, double time) override;
    bool keyboard(unsigned char k) override;

    void compute(bool update=false) override;
    glm::vec4& getColor() override { return m_color; }
    void toggleCtrlPts() override { m_displayCtrlPts = !m_displayCtrlPts; }
    glm::vec3 convertCoords(float winX, float winY);
    MyMesh::VertexIter m_v_iter;
    MyMesh::VertexIter m_oldv_iter;

private:
    // Shader program for rendering
    std::shared_ptr<Material> m_material;
    std::shared_ptr<Material> m_materialModified;
    std::shared_ptr<Material> m_materialLambert;
    std::shared_ptr<Material> m_materialNormal;
    std::shared_ptr<Material> m_materialParametric;
    std::shared_ptr<Material> m_materialTexture;

    Renderer* m_renderer;
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


    MyMesh m_myMesh;

    std::vector<unsigned int> m_indices;
    std::vector<Vertex> m_vertices;


};

/*------------------------------------------------------------------------------------------------------------------------*/


#endif // HELLO_MESH_H
