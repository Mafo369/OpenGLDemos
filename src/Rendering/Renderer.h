#ifndef RENDERER_H
#define RENDERER_H
#pragma once

#include "Geometry/Mesh.h"
#include "RenderObject.h"
#include "../demos/opengldemo.h"
#include "Shader.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "Framebuffer.h"
#include "../demos/camera.h"

typedef struct s_BloomMip
{
    glm::vec2 size;
    glm::ivec2 intSize;
    unsigned int texture;
} BloomMip;


class Renderer
{
public:
    Renderer();
    ~Renderer();
    void addRenderObject(RenderObject* ro);
    void addPointLight(glm::vec3 position, glm::vec3 color, float constant=1, float linear=0, float quadratic=0);
    void setDirLight(glm::vec3 direction, glm::vec3 color);
    void setMaterial(std::shared_ptr<Material> material);
    void setMaterialParams();
    void setLightMVP(glm::mat4 model, glm::mat4 view, glm::mat4 projection, unsigned int id);
    void setVP(glm::mat4 view, glm::mat4 projection);
    void setLight(PointLight* light, unsigned int id);
    void setCameraPosition(glm::vec3 position);
    void clearRenderObjects(); 
    std::shared_ptr<Material> getCurrentMaterial() { if(!m_roList.empty()) return m_roList[0]->getMaterial(); else return nullptr; }


    void setEnvMap(const std::string& path);
    bool hasCubeMap() { return m_hasCubeMap; }
    unsigned int getCubeMap() { return m_envCubeMap; }
    void setupBloom(int width, int height);
    void setupShadows(float cameraNearPlane, float cameraFarPlane);

    void depthOnlyPass(Camera* camera, glm::vec3& lightDir, int width, int height);

    void draw(VertexArray* vao, IndexBuffer* ebo, Shader* shader) const;
    void draw(Mesh* mesh, Shader* shader);
    void draw(Shader* shader);
    void draw();

    DirLightParams& getDirLight() { return m_dirLight; }

    std::vector<float>& getShadowCascadeLevels() { return m_shadowCascadeLevels; }
    unsigned int getLightDepthMaps() { return m_lightDepthMaps; }

private:
    std::vector<RenderObject*> m_roList;
    std::vector<RenderObject*> m_roLights;

    DirLightParams m_dirLight;

    unsigned int m_envMap;
    unsigned int m_envCubeMap;

    Shader* m_programBg;
    Mesh* m_cubeMesh;

    bool m_hasCubeMap;

    Framebuffer* m_fbo;
    Framebuffer* m_mipfbo;
    unsigned int m_fboTexture;
    unsigned int m_fboThTexture;
    unsigned int m_quadVAO;
    unsigned int m_quadVBO;
    unsigned int m_rbo;
    unsigned int m_attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    std::vector<BloomMip> m_mipChain;

    Shader* m_programDepth;
    Framebuffer* m_lightFBO;
    unsigned int m_matricesUBO;

    float m_cameraNearPlane;
    float m_cameraFarPlane;

    std::vector<float> m_shadowCascadeLevels;

    unsigned int m_lightDepthMaps;

    unsigned int depthMapResolution = 4096;

    std::vector<glm::mat4> m_lightMatrices;

    glm::mat4 m_view, m_projection;
};

std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview);

std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);

glm::mat4 getLightSpaceMatrix(Camera* camera, const float nearPlane, const float farPlane, float width, float height, glm::vec3 lightDir);

std::vector<glm::mat4> getLightSpaceMatrices(Camera* camera, std::vector<float>& shadowCascadeLevels, float nearPlane, float farPlane, float width, float height, glm::vec3 lightDir);


#endif
