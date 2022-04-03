#ifndef SHADER_H
#define SHADER_H
#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include "Material.h"

class Light;
  
struct ShaderProgramSource{
    std::string vertexSource;
    std::string fragmentSource;
};

class Shader
{
private:
    unsigned int m_rendererId;
public:
    // constructor reads and builds the shader
    Shader(const std::string& vertexFilepath, const std::string& fragmentFilepath); 
    ~Shader();

    void bind() const;
    void unbind() const;

    // utility uniform functions
    void setUniform4f(const std::string& name, const glm::vec4& v);
    void setUniform3f(const std::string& name, const glm::vec3& v);
    void setUniform1f(const std::string& name, float value);
    void setUniform1i(const std::string& name, int value);

    void setMVP(glm::mat4 _model, glm::mat4 _view, glm::mat4 _projection);
    void setLight(Light* light, unsigned int id);
    void setMaterialParams(MaterialParams params);
private:
    std::stringstream readShader(const std::string& filepath);
    ShaderProgramSource parseShader(const std::string& vertexFilepath, const std::string& fragmentFilepath);
    unsigned int compileShader(unsigned int type, const std::string& source);
    unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);
    int getUniformLocation(const std::string& name);
};

#endif
