#pragma once

#include "Renderer.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
  
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
private:
    std::stringstream readShader(const std::string& filepath);
    ShaderProgramSource parseShader(const std::string& vertexFilepath, const std::string& fragmentFilepath);
    unsigned int compileShader(unsigned int type, const std::string& source);
    unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);
    int getUniformLocation(const std::string& name);
};
  
