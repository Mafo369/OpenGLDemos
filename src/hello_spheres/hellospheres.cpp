#include "hellospheres.h"
#include <iostream>

#include "spheres.h"

/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------*/


#define deg2rad(x) float(M_PI)*(x)/180.f

static const char* vertexshader_source ="#version 410 core\n\
        layout (location = 0) in vec3 position;\n\
        uniform mat4 model;\n\
        uniform mat4 view;\n\
        uniform mat4 projection;\n\
        out vec3 normal;\n\
        out vec3 pos;\
        void main()\n\
        {\n\
            // Note that we read the multiplication from right to left\n\
            gl_Position = projection * view * model * vec4(position, 1.0f);\n\
            normal = normalize(position);\n\
            pos = position;\
        }\n";

static const char* fragmentshader_source ="#version 410 core\n\
        in vec3 normal;\n\
        in vec3 pos;\
        uniform int mode;\
        out vec4 color;\n\
        void main()\n\
        {\n\
//            color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n\
//            color = vec4(vec3(clamp(dot(normalize(normal), vec3(0,0,1)), 0, 1)), 1.0);\n\
            if (mode == 1)\
              color  = 20*vec4(vec3(1-length(pos)), 1);\
            else {\
              color = vec4(normalize(pos)*0.5+0.5, 1);\
            }\
        }\n";


void SimpleSpheres::Sphere::buildFrom(const Mesh &m) {
  for(const auto& v : m.vertices) {
    _vertices.emplace_back(v.x);
    _vertices.emplace_back(v.y);
    _vertices.emplace_back(v.z);
  }
  _indices = std::move(m.triangles);
  std::cout << "Spher with " << _indices.size()/3 << " triangles." << std::endl;
  // Initialize the geometry
  // 1. Generate geometry buffers
  glGenBuffers(1, &_vbo);
  glGenBuffers(1, &_ebo);
  glGenVertexArrays(1, &_vao);
  // 2. Bind Vertex Array Object
  glBindVertexArray(_vao);
  // 3. Copy our vertices array in a buffer for OpenGL to use
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, _vertices.size()*sizeof (GLfloat), _vertices.data(), GL_STATIC_DRAW);
  // 4. Copy our index array in a element buffer for OpenGL to use
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size()*sizeof (GLfloat), _indices.data(), GL_STATIC_DRAW);
  // 5. Then set our vertex attributes pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  //6. Unbind the VAO
  glBindVertexArray(0);

}

void SimpleSpheres::Sphere::draw() {
  glBindVertexArray(_vao);
  glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

SimpleSpheres::SimpleSpheres(int width, int height) : OpenGLDemo(width, height), _camera(nullptr) {
      Mesh m0;
      Mesh m1;
      Mesh m2;
      Mesh m3;
      Mesh m4;
      UVSphere(18, 10, m4);
      _parametric.buildFrom(m4);

      Icosahedron(m1);        // 20 triangles
      SubdivideMesh(m1, m2);  // 80 triangles
      SubdivideMesh(m2, m3);  // 320 triangls

      SubdivideMesh(m3, m0);
/*
      SubdivideMesh(m2, m3);
      SubdivideMesh(m3, m2);
  */
    _geodesic.buildFrom( m0 );


    _toDraw = &_parametric;
    _mode = 1;
    // Initialize shaders
    GLint success;
    GLchar infoLog[512]; // warning fixed size ... request for LOG_LENGTH!!!
    GLuint vertexshader, fragmentshader;

    // 1. Generate the shader
    vertexshader = glCreateShader(GL_VERTEX_SHADER);
    // 2. set the source
    glShaderSource(vertexshader, 1, &vertexshader_source, NULL);
    // 3. Compile
    glCompileShader(vertexshader);
    // 4. test for compile error
    glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexshader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentshader, 1, &fragmentshader_source, NULL);
    glCompileShader(fragmentshader);
    glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentshader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 1. Generate the program
    _program = glCreateProgram();
    // 2. Attach the shaders to the program
    glAttachShader(_program, vertexshader);
    glAttachShader(_program, fragmentshader);
    // 3. Link the program
    glLinkProgram(_program);
    // 4. Test for link errors
    glGetProgramiv(_program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(_program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::LINK_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexshader);
    glDeleteShader(fragmentshader);

    _camera.reset(new TrackballCamera(glm::vec3(0.f, 0.f, 3.f)));
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));
    _view = _camera->viewmatrix();


    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, 0.1f, 100.0f);

    _model = glm::mat4(1.0);
}

SimpleSpheres::~SimpleSpheres() {
    glDeleteProgram(_program);
    glDeleteBuffers(1, &_parametric._vbo);
    glDeleteBuffers(1, &_parametric._ebo);
    glDeleteVertexArrays(1, &_parametric._vao) ;
}

void SimpleSpheres::resize(int width, int height){
    OpenGLDemo::resize(width, height);
    _camera->setviewport(glm::vec4(0.f, 0.f, _width, _height));

}

void SimpleSpheres::draw() {
    OpenGLDemo::draw();

    glUseProgram(_program);

    _view = _camera->viewmatrix();
    _projection = glm::perspective(glm::radians(_camera->zoom()), float(_width) / _height, 0.1f, 100.0f);

    glUniformMatrix4fv( glGetUniformLocation(_program, "model"), 1, GL_FALSE, glm::value_ptr(_model));
    glUniformMatrix4fv( glGetUniformLocation(_program, "view"), 1, GL_FALSE, glm::value_ptr(_view));
    glUniformMatrix4fv( glGetUniformLocation(_program, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));

    glUniform1i( glGetUniformLocation(_program, "mode"), _mode );
    _toDraw->draw();
}

void SimpleSpheres::mouseclick(int button, float xpos, float ypos) {
    _button = button;
    _mousex = xpos;
    _mousey = ypos;
    _camera->processmouseclick(_button, xpos, ypos);
}

void SimpleSpheres::mousemove(float xpos, float ypos) {
    _camera->processmousemovement(_button, xpos, ypos, true);
}

void SimpleSpheres::mousewheel(float delta) {
    _camera->processmousescroll(delta);
}

void SimpleSpheres::keyboardmove(int key, double time) {
    _camera->processkeyboard(Camera_Movement(key), time);
}

bool SimpleSpheres::keyboard(unsigned char k) {
  switch ( k ) {
  case 'p' :
    _toDraw = &_parametric;
    return true;
  case 'g' :
    _toDraw = &_geodesic;
    return true;
  case 'm' :
    _mode = 1 - _mode;
    return true;
  }
  return OpenGLDemo::keyboard(k);
}
