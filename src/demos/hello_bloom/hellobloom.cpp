#include "hellobloom.h"
#include <iostream>
#include <math.h>
#include <memory>
#include <vector>
#include "Geometry/BezierCurve.h"
#include "Geometry/BezierSurface.h"
#include "Geometry/Cube.h"
#include "Geometry/Sphere.h"
#include "Geometry/Plane.h"
#include "Geometry/Mesh.h"
#include "../../Rendering/RenderObject.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>

#define deg2rad(x) float(M_PI)*(x)/180.f

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../../libs/stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model 
{
public:
    // model data 
    std::vector<Texture*> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh>    meshes;
    std::vector<RenderObject*> m_ros;
    std::string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model(std::string const &path, Shader* shader, glm::vec4 color,bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path, shader, color);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader &shader)
    {
        //for(unsigned int i = 0; i < meshes.size(); i++)
        //    meshes[i].Draw(shader);
    }
    
private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(std::string const &path, Shader* shader, glm::vec4 color)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene, shader, color);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene, Shader* shader, glm::vec4 color)
    {
        // process each mesh located at the current node
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            std::vector<Texture*> textures;
            Mesh* pMesh = processMesh(mesh, scene, color, textures);
            MaterialParams matParams;
            matParams.texDiffuse = 4;
            matParams.texSpecular = 5;
            matParams.metallic = 0.9;
            matParams.roughness = 0.9;
            std::shared_ptr<Material> material;
            int idx = -1;
            for(unsigned int j = 0; j < textures_loaded.size(); j++) {
              if(textures_loaded[i]->m_filePath.compare("texture_diffuse")){
                  idx = i;
              }
            }
            if(idx != -1){
              material = std::make_shared<Material>(shader, matParams, textures_loaded[0], textures_loaded[0]);
            }
            else
            {
              material = std::make_shared<Material>(shader, matParams);
            }
            RenderObject* ro = new RenderObject(pMesh, material);
            m_ros.push_back(ro);
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene, shader, color);
        }

    }

    Mesh* processMesh(aiMesh *mesh, const aiScene *scene, glm::vec4 color, std::vector<Texture*>& textures)
    {
        // data to fill
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.m_position = vector;

            if (mesh->HasVertexColors(i))
            {
                glm::vec4 vertexColor;
                vertexColor.r = mesh->mColors[i]->r;
                vertexColor.g = mesh->mColors[i]->g;
                vertexColor.b = mesh->mColors[i]->b;
                vertexColor.a = mesh->mColors[i]->a;
                vertex.m_color = vertexColor;
            }
            else
            {
                vertex.m_color = color; 
            }
            
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.m_normal = vector;
            }
            // texture coordinates
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = 1.f - mesh->mTextureCoords[0][i].y;
                vertex.m_texCoords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.m_tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.m_bitTangent = vector;
            }
            else
                vertex.m_texCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);        
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 1. diffuse maps
        std::vector<Texture*> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        //// 2. specular maps
        //std::vector<Texture*> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        //textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        //// 3. normal maps
        //std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        //textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        //// 4. height maps
        //std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        //textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
        // return a mesh object created from the extracted mesh data
        return new Mesh(vertices, indices, GL_TRIANGLES);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    std::vector<Texture*> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
    {
      std::vector<Texture*> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            std::string tmp(str.C_Str());
            std::replace(tmp.begin(), tmp.end(), '\\', '/');
            str.Set(tmp);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for(unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if(std::strcmp(textures_loaded[j]->m_filePath.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if(!skip)
            {   // if texture hasn't been loaded already, load it
                std::string filename = std::string(str.C_Str());
                filename = this->directory + '/' + filename;
                Texture* texture = new Texture(filename);
                texture->type = typeName;
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
            }
        }
        return textures;
    }
};

BloomDemo::BloomDemo(int width, int height, ImVec4 clearColor) : OpenGLDemo(width, height, clearColor), _activecamera(0), _camera(nullptr) {
    /*** Initialise geometric data ***/
    m_color = glm::vec4(45.1f, 0.3f, 0.3f, 1.f);

    /*** Initialise renderer ***/
    m_renderer = new Renderer();

    /*** Create render objects ***/

    // Textures
    Texture* texture = new Texture("Assets/container2.png");
    Texture* textureSpecular = new Texture("Assets/container2_specular.png");
    
    // Shaders
    Shader* program = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/ShadowMapTest.frag.glsl");
    Shader* programMicrofacet = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/Microfacet.frag.glsl");
    Shader* programModified = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/MicrofacetModified.frag.glsl");
    Shader* programLambert = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/Lambert.frag.glsl");
    Shader* programNormal = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/Normals.frag.glsl");
    Shader* programParametric = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/Parametric.frag.glsl");
    Shader* programTexture = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/MicrofacetTexture.frag.glsl");
    Shader* programTexture1 = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/MicrofacetTexture.frag.glsl");
    Shader* programBasic = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/Basic.frag.glsl");
    Shader* programSpecular = 
        new Shader("Shaders/Camera.vert.glsl", "Shaders/Specular.frag.glsl");

    m_renderer->setEnvMap("/home/mafo/dev/Raytracer/assets/Alexs_Apt_2k.hdr");

    m_programQuad = 
        new Shader("Shaders/Sample.vert.glsl", "Shaders/Quad.frag.glsl");
    m_programTh = 
        new Shader("Shaders/Sample.vert.glsl", "Shaders/Threshold.frag.glsl");
    m_programDown = 
        new Shader("Shaders/Sample.vert.glsl", "Shaders/Downsample.frag.glsl");
    m_programUp = 
        new Shader("Shaders/Sample.vert.glsl", "Shaders/Upsample.frag.glsl");

    m_programQuad->bind();
    m_programQuad->setUniform1i("screenTexture", 0);
    m_programQuad->setUniform1i("origTexture", 1);
    m_programQuad->setUniform1f("exposure", m_exposure);
    m_programQuad->unbind();

    m_programTh->bind();
    m_programTh->setUniform1i("screenTexture", 0);
    m_programTh->setUniform1f("threshold", m_threshold);
    m_programTh->unbind();

    m_programDown->bind();
    m_programDown->setUniform1i("srcTexture", 0);
    m_programDown->unbind();

    m_programUp->bind();
    m_programUp->setUniform1i("srcTexture", 0);
    m_programUp->unbind();

    // Materials
    MaterialParams matParams;
    matParams.texDiffuse = 2;
    matParams.texSpecular = 3;
    matParams.metallic = 0.939;
    matParams.roughness = 0.276;
    m_material = std::make_shared<Material>(program, matParams);
    m_materialSpecular = std::make_shared<Material>(programSpecular, matParams);
    m_materialMicrofacet = std::make_shared<Material>(programMicrofacet, matParams);
    m_materialBasic = std::make_shared<Material>(programBasic, matParams);
    m_materialModified = std::make_shared<Material>(programModified, matParams);
    m_materialLambert = std::make_shared<Material>(programLambert);
    m_materialNormal = std::make_shared<Material>(programNormal);
    m_materialParametric = std::make_shared<Material>(programParametric);
    m_materialTexture = std::make_shared<Material>(programTexture1, matParams, texture, textureSpecular);
    
    m_first = false;

    /*** Create Camera ***/
    _cameraselector.push_back( []()->Camera*{return new EulerCamera(glm::vec3(-10.09f, 2.47f, 7.6f), {0,1.f,0.f}, -50.f, 0.f);} );
    _cameraselector.push_back( []()->Camera*{return new TrackballCamera(glm::vec3(0.f, 0.f, 1.f));} );
    _camera.reset(_cameraselector[_activecamera]());
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _view = _camera->viewmatrix();
    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, _camera->getNearPlane(), _camera->getFarPlane());

    /*** Add lights to renderer ***/
    m_renderer->addPointLight(glm::vec3(3.f, 6.f, 0.f), glm::vec3(50));
    m_renderer->addPointLight(glm::vec3(0.f, 6.f, 0.f), glm::vec3(15.0f, 0, 0));
    m_renderer->addPointLight(glm::vec3(-3.f, 6.f, 0.0f), glm::vec3(0.0f, 15.0f, 0.f));
    m_renderer->addPointLight(glm::vec3(-6.f, 6.f, 0.0f), glm::vec3(0.0f, 0.0f, 25.f));
    m_renderer->addPointLight(glm::vec3(0.f, 1.f, 0.0f), glm::vec3(5.f));

    m_renderer->setDirLight(lightDir, glm::vec3(1.f));

    m_fbo = new Framebuffer();
    m_fbo->bind();
    
    int hdrWidth = _width;
    int hdrHeight = _height;

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

    auto loadedModel = Model("/home/mafo/dev/Raytracer/assets/Suzanne.obj", programBasic, m_color);
    for(auto& ro : loadedModel.m_ros){
      auto monkeyTransform = glm::translate(glm::mat4(1.f), glm::vec3(0, 3, -2)) * glm::scale(glm::mat4(1.f), glm::vec3(0.68f));
      ro->setTransform(monkeyTransform);
      m_renderer->addRenderObject(ro);
    }

    loadedModel = Model("Assets/hover-bike/hover_bike/scene.gltf", programTexture, {0,0,0,1});
    for(auto& ro : loadedModel.m_ros){
      auto monkeyTransform = glm::translate(glm::mat4(1.f), glm::vec3(6, 0, -14)) * glm::rotate(glm::mat4(1.f), deg2rad(90), glm::vec3(0,0,1)) * glm::scale(glm::mat4(1.f), glm::vec3(0.5f));
      monkeyTransform = monkeyTransform * glm::rotate(glm::mat4(1.f), deg2rad(90), glm::vec3(0,1,0));
      ro->setTransform(monkeyTransform);
      m_renderer->addRenderObject(ro);
    }

    m_mesh = new Plane({0,0,0,1});

    Mesh* cubeMesh = new Cube({0.1,0.1,0.8,1});
    auto cubeRo = new RenderObject(cubeMesh, m_materialTexture);
    auto cubeT =  glm::scale(glm::mat4(1.f), glm::vec3(0.8f)) * glm::translate(glm::mat4(1.f), glm::vec3(3, 2, -1)) * glm::rotate(glm::mat4(1.f), 90.f, glm::vec3(1,1,0)) ;
    cubeRo->setTransform(cubeT);
    m_renderer->addRenderObject(cubeRo);

    auto planeMesh = new Plane({0.855,0.855,0.855,1});
    auto planeRo = new RenderObject(planeMesh, m_material);
    auto planeT = glm::translate(glm::mat4(1.f), glm::vec3(0,-1.2f, 0)) * glm::rotate(glm::mat4(1.f), deg2rad(90.f), glm::vec3(-1,0,0)) * glm::scale(glm::mat4(1.f), glm::vec3(50.f));
    planeRo->setTransform(planeT);
    m_renderer->addRenderObject(planeRo);

    auto sphereMesh = new Sphere({0.1, 0.7, 0.1, 1.f});
    auto sphereRo = new RenderObject(sphereMesh, m_materialSpecular);
    auto sphereT = glm::translate(glm::mat4(1.f), glm::vec3(-3, 0, -3));
    sphereRo->setTransform(sphereT);
    m_renderer->addRenderObject(sphereRo);

    m_particle = new VerletParticle(sphereMesh, m_materialMicrofacet, glm::vec3(-8,10,-2), false);
    m_particle->addConstraint(Aabb(glm::vec3(-50, -0.2, -50), glm::vec3(50, 40, 50)));
    auto gravity = glm::vec3(0.0, -0.05, 0.0);
    m_particle->addBehavior(gravity);
    m_renderer->addRenderObject(m_particle);

    std::vector<std::vector<glm::vec3>> controlPoints = {
      {{-0.5f, -0.5f, 0.0f}, {0.f, 0.f, 0.0f}, {0.5f, 0.f, 0.0f}, {1.f, -0.5f, 0.0f}},
      {{-0.5f, -0.5f, 0.5f}, {0.f, 0.f, 0.5f}, {0.5f, 0.5f, 0.5f}, {1.f, -0.5f, 0.5f}},
      {{-0.5f,  -0.5f, 1.0f}, {0.f, 0.5f, 1.0f}, {0.83f, 1.3f, 1.0f}, {1.f, -0.3f, 1.0f}},
      {{-0.5f,  0.f, 1.5f}, {0.f, 0.f, 1.5f}, {0.5f, -1.0f, 1.5f}, {1.f, 0.3f, 1.5f}}
    };
    Mesh* mesh = new BezierSurface(controlPoints, 100, 100, glm::vec4(1.0f, 0.2f, 0.2f, 1.f));
    auto ro = new RenderObject(mesh, m_materialMicrofacet);
    auto bezierSurfaceT = glm::scale(glm::mat4(1.f), glm::vec3(3)) * glm::translate(glm::mat4(1.f), glm::vec3(-2, 0.5, -5));
    ro->setTransform(bezierSurfaceT);
    m_renderer->addRenderObject(ro);

    m_renderer->setupShadows(_camera->getNearPlane(), _camera->getFarPlane());

    m_material->getShader()->bind();
    m_material->getShader()->setUniform1i("shadowMap", 0);
    m_material->getShader()->setUniform1i("envMap", 1);
    m_material->getShader()->setUniform1f("farPlane", _camera->getFarPlane());
    auto& shadowCascadeLevels = m_renderer->getShadowCascadeLevels();
    m_material->getShader()->setUniform1i("cascadeCount", shadowCascadeLevels.size());
    for (size_t i = 0; i < shadowCascadeLevels.size(); ++i){
      m_material->getShader()->setUniform1f("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
    }  

    m_materialSpecular->getShader()->bind();
    m_materialSpecular->getShader()->setUniform1i("shadowMap", 0);
    m_materialSpecular->getShader()->setUniform1i("envMap", 1);
    m_materialSpecular->getShader()->setUniform1f("farPlane", _camera->getFarPlane());
    m_materialSpecular->getShader()->setUniform1i("cascadeCount", shadowCascadeLevels.size());
    for (size_t i = 0; i < shadowCascadeLevels.size(); ++i){
      m_materialSpecular->getShader()->setUniform1f("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
    }  

    m_materialLambert->getShader()->bind();
    m_materialLambert->getShader()->setUniform1i("shadowMap", 0);
    m_materialLambert->getShader()->setUniform1i("envMap", 1);
    m_materialLambert->getShader()->setUniform1f("farPlane", _camera->getFarPlane());
    m_materialLambert->getShader()->setUniform1i("cascadeCount", shadowCascadeLevels.size());
    for (size_t i = 0; i < shadowCascadeLevels.size(); ++i){
      m_materialLambert->getShader()->setUniform1f("cascadePlaneDistances[" + std::to_string(i) + "]", shadowCascadeLevels[i]);
    }  
}

BloomDemo::~BloomDemo() {
    for (unsigned int i = 0; i < m_mipChain.size(); i++) {
        glDeleteTextures(1, &m_mipChain[i].texture);
        m_mipChain[i].texture = 0;
    }
    delete m_fbo;
    delete m_mipfbo;
    delete m_renderer;
}

void BloomDemo::compute() {
    m_programQuad->bind();
    m_programQuad->setUniform1f("exposure", m_exposure);
    m_programQuad->unbind();

    m_programTh->bind();
    m_programTh->setUniform1f("threshold", m_threshold);
    m_programTh->unbind();

    //m_currentRo->getMesh()->setColor(m_color);
    m_renderer->setDirLight(lightDir, glm::vec3(1));
}

void BloomDemo::resize(int width, int height){
    OpenGLDemo::resize(width, height);
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
}

template <typename T>
void printmatrix(T * ptr) {
    std::cerr << std::fixed;
    for (int i=0; i<4; ++i) {
        std::cerr << "( ";
        for (int j=0; j<4; ++j) {
            std::cerr << *ptr++ << " ";
        }
        std::cerr << ") ";
    }
    std::cerr  << std::defaultfloat << std::endl;
}

void BloomDemo::draw() {
    OpenGLDemo::draw();

    /*** Compute new camera transform ***/
    _view = _camera->viewmatrix();
    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, _camera->getNearPlane(), _camera->getFarPlane());

    // depth pass for shadow mapping
    m_renderer->setCameraPosition(_camera->position());
    m_renderer->setVP(_view, _projection);
    m_renderer->depthOnlyPass(_camera.get(), lightDir, _width, _height);

    // render scene as normal using the generated depth/shadow map  
    m_fbo->bind();
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    if (_drawfill)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glViewport(0, 0, _width, _height);

    /*** Update Material ***/
    m_renderer->setMaterialParams();
   
    /*** Update scene ***/
    m_renderer->setVP(_view, _projection);
    m_renderer->setCameraPosition(_camera->position());
    m_particle->update();
    m_renderer->draw();	

    glDrawBuffer(GL_COLOR_ATTACHMENT1);

    m_programTh->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    m_programTh->setUniform1f("threshold", m_threshold);
    m_renderer->draw(m_mesh, m_programTh);
    m_programTh->unbind();

    m_mipfbo->bind();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_DEPTH_TEST);

    m_programDown->bind();
    m_programDown->setUniform1f("srcResolutionX", (float)_width);
    m_programDown->setUniform1f("srcResolutionY", (float)_height);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fboThTexture);

    for(int i = 0; i < (int)m_mipChain.size(); i++){
        const BloomMip& mip = m_mipChain[i];
        glViewport(0, 0, mip.size.x, mip.size.y);

        m_mipfbo->attachTexture2D(GL_COLOR_ATTACHMENT0, mip.texture);

        // Render screen-filled quad of resolution of current mip
        m_renderer->draw(m_mesh, m_programDown);

        // Set current mip resolution as srcResolution for next iteration
        m_programDown->bind();
        m_programDown->setUniform1f("srcResolutionX", mip.size.x);
        m_programDown->setUniform1f("srcResolutionY", mip.size.y);
        glBindTexture(GL_TEXTURE_2D, mip.texture);
        m_programDown->unbind();
    }

    m_programUp->bind();
    m_programUp->setUniform1f("filterRadius", 0.005f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    for (int i = (int)m_mipChain.size() - 1; i > 0; i--)
    {
        const BloomMip& mip = m_mipChain[i];
        const BloomMip& nextMip = m_mipChain[i-1];

        // Bind viewport and texture from where to read
        m_programUp->bind();
        glBindTexture(GL_TEXTURE_2D, mip.texture);
        m_programUp->unbind();

        // Set framebuffer render target (we write to this texture)
        glViewport(0, 0, nextMip.size.x, nextMip.size.y);
        m_mipfbo->attachTexture(GL_COLOR_ATTACHMENT0, nextMip.texture);

        m_renderer->draw(m_mesh, m_programUp);
    }

    // Disable additive blending
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Restore if this was default
    glDisable(GL_BLEND);

    // second pass
    m_mipfbo->unbind();
    glViewport(0,0, _width, _height);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.9f, 0.2f, 0.2f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    m_programQuad->bind();
    m_programQuad->setUniform1f("exposure", m_exposure);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_mipChain[0].texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_fboTexture);
    m_renderer->draw(m_mesh, m_programQuad);
}

void BloomDemo::mouseclick(int button, float xpos, float ypos) {
    _button = button;
    _mousex = xpos;
    _mousey = ypos;
    _camera->processmouseclick(_button, xpos, ypos);
}

void BloomDemo::mousemove(float xpos, float ypos) {
    _camera->processmousemovement(_button, xpos, ypos, true);
}

void BloomDemo::mousewheel(float delta) {
    _camera->processmousescroll(delta);
}

void BloomDemo::keyboardmove(int key, double time) {
    _camera->processkeyboard(Camera_Movement(key), time);
}

bool BloomDemo::keyboard(unsigned char k) {
    switch(k) {
        case 'p':
            _activecamera = (_activecamera+1)%2;
            _camera.reset(_cameraselector[_activecamera]());
            _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
            return true;
        case 'c' :
            m_renderer->setMaterial(m_material);
            return true;
        case 'm' :
            m_renderer->setMaterial(m_materialModified);
            return true;
        case 'n' :
            m_renderer->setMaterial(m_materialNormal);
            return true;
        case 'u' :
            m_renderer->setMaterial(m_materialParametric);
            return true;
        case 'l' :
            m_renderer->setMaterial(m_materialLambert);
            return true;
        case 't' :
            m_renderer->setMaterial(m_materialTexture);
            return true;
        case 'b' :
            m_renderer->setMaterial(m_materialBasic);
            return true;
        default:
            return false;
    }
}
