#include "Shader.h"
#include "Renderer.h"

Shader::Shader(const std::string& vertexFilepath, const std::string& fragmentFilepath) 
{
    ShaderProgramSource source = parseShader(vertexFilepath, fragmentFilepath);
    if(source.vertexSource.empty())
        std::cout << "Error: Invalid vertex shader!" << std::endl;
    if(source.fragmentSource.empty())
        std::cout << "Error: Invalid fragment shader!" << std::endl;
    m_rendererId = createShader( source.vertexSource, source.fragmentSource );
}

Shader::~Shader(){
    glAssert(glDeleteProgram(m_rendererId));
}

void Shader::bind() const{
    glAssert(glUseProgram(m_rendererId));
}

void Shader::unbind() const{
    glAssert(glUseProgram(0));
}

void Shader::setUniform4f(const std::string& name, const glm::vec4& v){
    glAssert(glUniform4f(getUniformLocation(name), v.x, v.y, v.z, v.w));
}

void Shader::setUniform3f(const std::string& name, const glm::vec3& v){
    glAssert(glUniform3f(getUniformLocation(name), v.x, v.y, v.z));
}

void Shader::setUniform1f(const std::string& name, float value){
    glAssert(glUniform1f(getUniformLocation(name), value));
}
    
void Shader::setMVP(glm::mat4 _model, glm::mat4 _view, glm::mat4 _projection){
    bind();
    glUniformMatrix4fv( getUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(_model));
    glUniformMatrix4fv( getUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(_view));
    glUniformMatrix4fv( getUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(_projection));
    unbind();
}

void Shader::setLight(Light light){
    bind();
    setUniform3f( "light.position", light.position);
    setUniform3f( "light.color", light.color);
    setUniform1f("light.attenuation.constant", light.attenuation.constant);
    setUniform1f("light.attenuation.linear", light.attenuation.linear);
    setUniform1f("light.attenuation.quadratic", light.attenuation.quadratic);
    unbind();
}

int Shader::getUniformLocation(const std::string& name){
    glAssert(int location = glGetUniformLocation(m_rendererId, name.c_str()));
    if(location == -1)
        std::cout << "Warning: uniform " << name << "doesn't exist!" << std::endl;
    return location;
}

std::stringstream Shader::readShader(const std::string& filepath){
    std::ifstream stream(filepath);
    std::stringstream ss;
    std::string line;
    while(getline(stream, line)){
        ss << line << '\n';
    }
    return ss;
}

ShaderProgramSource Shader::parseShader(const std::string& vertexFilepath,
                                        const std::string& fragmentFilepath){
    return { readShader(vertexFilepath).str(), readShader(fragmentFilepath).str() };
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source){
    glAssert(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();
    glAssert(glShaderSource(id, 1, &src, nullptr));
    glAssert(glCompileShader(id));

    int result;
    glAssert(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if(result == GL_FALSE){
        int length;
        glAssert(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* msg = (char*)alloca(length * sizeof(char));
        glAssert(glGetShaderInfoLog(id, length, &length, msg));
        std::cout << "Failed to compile " << 
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
        std::cout << msg << std::endl;
        glAssert(glDeleteShader(id));
        return 0;
    }
    return id;
}

unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader){
    glAssert(unsigned int program = glCreateProgram());
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAssert(glAttachShader(program, vs));
    glAssert(glAttachShader(program, fs));
    glAssert(glLinkProgram(program));
    int result;
    glAssert(glGetProgramiv(program, GL_LINK_STATUS, &result));
    if(result == GL_FALSE){
        int length;
        glAssert(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
        char* msg = (char*)alloca(length * sizeof(char));
        glAssert(glGetProgramInfoLog(program, length, &length, msg));
        std::cout << "Failed to link program!" << std::endl;
        std::cout << msg << std::endl;
        glDeleteProgram(program);
        return 0;
    }
    glAssert(glValidateProgram(program));

    glAssert(glDeleteShader(vs));
    glAssert(glDeleteShader(fs));

    return program;
}
