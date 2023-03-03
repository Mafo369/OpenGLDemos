#include "Renderer.h"
#include "Framebuffer.h"
#include "../Geometry/Cube.h"
#include "../../../libs/stb_image.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& projview)
{
    const auto inv = glm::inverse(projview);

    std::vector<glm::vec4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}

std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
{
    return getFrustumCornersWorldSpace(proj * view);
}

glm::mat4 getLightSpaceMatrix(Camera* camera, const float nearPlane, const float farPlane, float width, float height, glm::vec3 lightDir)
{
    const auto proj = glm::perspective(
        glm::radians(camera->zoom()), width / height, nearPlane,
        farPlane);
    const auto corners = getFrustumCornersWorldSpace(proj, camera->viewmatrix());

    glm::vec3 center = glm::vec3(0, 0, 0);
    for (const auto& v : corners)
    {
        center += glm::vec3(v);
    }
    center /= corners.size();

    const auto lightView = glm::lookAt(center + lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto& v : corners)
    {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    // Tune this parameter according to the scene
    constexpr float zMult = 10.0f;
    if (minZ < 0)
    {
        minZ *= zMult;
    }
    else
    {
        minZ /= zMult;
    }
    if (maxZ < 0)
    {
        maxZ /= zMult;
    }
    else
    {
        maxZ *= zMult;
    }

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

    return lightProjection * lightView;
}

std::vector<glm::mat4> getLightSpaceMatrices(Camera* camera, std::vector<float>& shadowCascadeLevels, float nearPlane, float farPlane, float width, float height, glm::vec3 lightDir)
{
    std::vector<glm::mat4> ret;
    for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
    {
        if (i == 0)
        {
            ret.push_back(getLightSpaceMatrix(camera, nearPlane, shadowCascadeLevels[i], width, height, lightDir));
        }
        else if (i < shadowCascadeLevels.size())
        {
            ret.push_back(getLightSpaceMatrix(camera, shadowCascadeLevels[i - 1], shadowCascadeLevels[i], width, height, lightDir));
        }
        else
        {
            ret.push_back(getLightSpaceMatrix(camera, shadowCascadeLevels[i - 1], farPlane, width, height, lightDir));
        }
    }
    return ret;
}


Renderer::Renderer(){
  m_hasCubeMap = false;
  m_dirLight.direction = glm::vec3(0,-1,0);
  m_dirLight.color = glm::vec3(0);
}

Renderer::~Renderer(){
    for(auto& l : m_roLights)
        delete l;
}

void Renderer::setEnvMap(const std::string& path) {
    stbi_set_flip_vertically_on_load(true);
    int cmWidth, cmHeight, nrComponents;
    float *data = stbi_loadf(path.c_str(), &cmWidth, &cmHeight, &nrComponents, 0);
    if (data)
    {
        glGenTextures(1, &m_envMap);
        glBindTexture(GL_TEXTURE_2D, m_envMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, cmWidth, cmHeight, 0, GL_RGB, GL_FLOAT, data); 

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load environnement map." << std::endl;
        return ;
    }  

    m_cubeMesh = new Cube({0.1,0.1,0.8,1});

    Shader* programCube =
        new Shader("Shaders/Cubemap.vert.glsl", "Shaders/Cubemap.frag.glsl");
    m_programBg =
        new Shader("Shaders/Background.vert.glsl", "Shaders/Background.frag.glsl");

    programCube->bind();
    programCube->setUniform1i("equirectangularMap", 0);
    programCube->unbind();

    m_programBg->bind();
    m_programBg->setUniform1i("environmentMap", 0);
    m_programBg->unbind();

    Framebuffer* captureFbo = new Framebuffer();
    unsigned int captureRBO;
    glGenRenderbuffers(1, &captureRBO);

    captureFbo->bind();

    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, cmWidth, cmWidth);
    captureFbo->attachRenderbuffer(GL_DEPTH_ATTACHMENT, captureRBO);

    glGenTextures(1, &m_envCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubeMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 
                     cmWidth, cmWidth, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = 
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // convert HDR equirectangular environment map to cubemap equivalent
    programCube->bind();
    programCube->setUniform1i("equirectangularMap", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_envMap);

    glViewport(0, 0, cmWidth, cmWidth); // don't forget to configure the viewport to the capture dimensions.
    captureFbo->bind();
    for (unsigned int i = 0; i < 6; ++i)
    {
        programCube->setMVP(glm::mat4(1.f), captureViews[i], captureProjection);
        captureFbo->attachCubemapTexture2D(GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_envCubeMap);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw(m_cubeMesh, programCube);
    }
    captureFbo->unbind();

    m_hasCubeMap = true;
}

void Renderer::setupBloom(int width, int height) {
    m_fbo = new Framebuffer();
    m_fbo->bind();
    
    int hdrWidth = width;
    int hdrHeight = height;

    glGenTextures(1, &m_fboTexture);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, hdrWidth, hdrHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach it to currently bound framebuffer object
    m_fbo->attachTexture2D(GL_COLOR_ATTACHMENT0, m_fboTexture);

    glGenTextures(1, &m_fboThTexture);
    glBindTexture(GL_TEXTURE_2D, m_fboThTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, hdrWidth, hdrHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach it to currently bound framebuffer object
    m_fbo->attachTexture2D(GL_COLOR_ATTACHMENT1, m_fboThTexture);

    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, hdrWidth, hdrHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
    m_fbo->attachRenderbuffer(GL_DEPTH_STENCIL_ATTACHMENT, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    m_fbo->unbind();

    m_mipfbo = new Framebuffer();
    m_mipfbo->bind();

    glm::vec2 mipSize((float)hdrWidth, (float)hdrHeight);
    glm::ivec2 mipIntSize((int)hdrWidth, (int)hdrHeight);
    // Safety check
    if (hdrWidth > (int)INT_MAX || hdrHeight > (int)INT_MAX) {
        std::cerr << "Window size conversion overflow - cannot build bloom FBO!\n";
    }

    for(unsigned int i = 0; i < 6; i++){
      BloomMip mip;

      mipSize *= 0.5f;
      mipIntSize /= 2;
      mip.size = mipSize;
      mip.intSize = mipIntSize;

      // generate texture
      glGenTextures(1, &mip.texture);
      glBindTexture(GL_TEXTURE_2D, mip.texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, (int)mipSize.x, (int)mipSize.y, 0, GL_RGB, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

      std::cout << "Created bloom mip " << mipIntSize.x << 'x' << mipIntSize.y << std::endl;
      m_mipChain.emplace_back(mip);
    }

    // attach it to currently bound framebuffer object
    m_mipfbo->attachTexture2D(GL_COLOR_ATTACHMENT0, m_mipChain[0].texture);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    m_mipfbo->unbind();

}

void Renderer::setupShadows(float cameraNearPlane, float cameraFarPlane) {
    m_programDepth = 
        new Shader("Shaders/ShadowMap.vert.glsl", "Shaders/ShadowMap.frag.glsl", "Shaders/ShadowMap.geom.glsl");

    m_shadowCascadeLevels = { cameraFarPlane / 50.0f, cameraFarPlane / 25.0f, cameraFarPlane / 10.0f, cameraFarPlane / 2.0f };
    m_cameraNearPlane = cameraNearPlane;
    m_cameraFarPlane = cameraFarPlane;

    // configure light FBO
    // -----------------------
    m_lightFBO = new Framebuffer();

    glGenTextures(1, &m_lightDepthMaps);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_lightDepthMaps);
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, depthMapResolution, depthMapResolution, int(m_shadowCascadeLevels.size()) + 1,
        0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

    m_lightFBO->bind();
    m_lightFBO->attachTexture(GL_DEPTH_ATTACHMENT, m_lightDepthMaps);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    m_lightFBO->unbind();

    // configure UBO
    // --------------------
    glGenBuffers(1, &m_matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 3, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

}

void Renderer::depthOnlyPass(Camera* camera, glm::vec3& lightDir, int width, int height) {
    glEnable(GL_DEPTH_TEST);
    // 0. UBO setup
    const auto lightMatrices = getLightSpaceMatrices(camera, m_shadowCascadeLevels, m_cameraNearPlane, m_cameraFarPlane, width, height, lightDir);
    glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUBO);
    for (size_t i = 0; i < lightMatrices.size(); ++i)
    {
        glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMatrices[i]);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    m_programDepth->bind();

    m_lightFBO->bind();
    glViewport(0, 0, depthMapResolution, depthMapResolution);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);  // peter panning
    setCameraPosition(camera->position());
    draw(m_programDepth);
    glCullFace(GL_BACK);
    m_lightFBO->unbind();

    // reset viewport
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::draw(VertexArray *vao, IndexBuffer *ebo, Shader* shader) const{
    shader->bind();
    vao->bind();
    ebo->bind();
    glAssert(glDrawElements(ebo->getMode(), ebo->getCount(), GL_UNSIGNED_INT, nullptr));
    vao->unbind();
}

void Renderer::draw(Mesh* mesh, Shader* shader){
    draw(mesh->m_vao, mesh->m_ebo, shader);
}

void Renderer::draw() {
    for(auto& ro : m_roList) {
        Mesh* mesh = ro->getMesh();
        ro->getMaterial()->getShader()->setMVP(ro->getTransform(), m_view, m_projection);
        draw(mesh->m_vao, mesh->m_ebo, ro->getMaterial()->getShader());
    }
    for(unsigned int i = 0; i < m_roLights.size(); i++) {
        auto l = static_cast<PointLight*>(m_roLights[i]);
        setLightMVP(l->getModel(), m_view, m_projection , i);
        setLight(l, i);
        Mesh* mesh = l->getMesh();
        draw(mesh->m_vao, mesh->m_ebo, l->getMaterial()->getShader());
    }

    if(m_hasCubeMap){
      m_programBg->bind();
      m_programBg->setMVP(glm::mat4(1.f), m_view, m_projection);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubeMap);
      draw(m_cubeMesh, m_programBg);
    }
}

void Renderer::draw(Shader* shader) {
    for(auto& ro : m_roList) {
        Mesh* mesh = ro->getMesh();
        shader->setMVP(ro->getTransform(), m_view, m_projection);
        draw(mesh->m_vao, mesh->m_ebo, shader);
    }
    for(auto& ro : m_roLights) {
        Mesh* mesh = ro->getMesh();
        draw(mesh->m_vao, mesh->m_ebo, ro->getMaterial()->getShader());
    }
}

void Renderer::addRenderObject(RenderObject* ro){
    m_roList.push_back(ro);
}

void Renderer::addPointLight(glm::vec3 position, glm::vec3 color, float constant, float linear, float quadratic) {
  PointLightParams params;
  params.position = position;
  params.color = color;
  params.constant = constant;
  params.linear = linear;
  params.quadratic = quadratic;
  m_roLights.push_back(new PointLight(params));
}

void Renderer::setDirLight(glm::vec3 direction, glm::vec3 color) {
  m_dirLight.direction = direction;
  m_dirLight.color = color;
}

void Renderer::setMaterial(std::shared_ptr<Material> material){
    for(auto& ro: m_roList){
        ro->setMaterial(material);
    }
}

void Renderer::setMaterialParams(){
    for(auto& ro : m_roList){
        auto material = ro->getMaterial();
        if(material->hasParams()){
            auto materialParams = material->getParams();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D_ARRAY, getLightDepthMaps());
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, getCubeMap());
            if(material->hasTexture()){
                auto texture = material->getTexture();
                auto textureSpecular = material->getTextureSpecular();
                texture->bind(2);
                textureSpecular->bind(3);
                material->getShader()->setMaterialParams(materialParams);
            }
            else
                material->getShader()->setMaterialParams(materialParams);
        }

    }
}

void Renderer::setLightMVP(glm::mat4 model, glm::mat4 view, glm::mat4 projection, unsigned int id){
    m_roLights[id]->getMaterial()->getShader()->setMVP(model, view, projection);
}

void Renderer::setVP(glm::mat4 view, glm::mat4 projection){
    m_view = view;
    m_projection = projection;
}

void Renderer::setLight(PointLight* light, unsigned int id){
    for(auto& ro: m_roList){
        ro->getMaterial()->getShader()->setPointLight(light, id);
        ro->getMaterial()->getShader()->setDirLight(m_dirLight.direction, m_dirLight.color);
    }
}

void Renderer::setCameraPosition(glm::vec3 position){
    for(auto& ro: m_roList){
        ro->getMaterial()->getShader()->setCameraPosition(position);
    }
}

void Renderer::clearRenderObjects(){
    for(auto& ro : m_roList){
        delete ro;
    }
    m_roList.clear();
}
