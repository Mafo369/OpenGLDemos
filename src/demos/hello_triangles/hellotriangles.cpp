#include "hellotriangles.h"

SimpleTriangle::SimpleTriangle( int width, int height, ImVec4 clearColor ) :
    OpenGLDemo( width, height, clearColor ) {

    auto lookfrom = glm::vec3( 0., -70, 15 );
    auto lookat   = glm::vec3( 2, 0, 3 );
    // setCameraFOV(scene, from, at, vec3(0, 0, 1), 30.f, float(WIDTH), float(HEIGHT), 0.01,
    // distance(from, at));
    auto vup           = glm::vec3( 0, 0, 1 );
    float vfov         = 30.f;
    float aspect_ratio = 800.f / 600;

    glm::vec3 zdir = normalize( lookat - lookfrom );
    glm::vec3 xdir = cross( zdir, vup );
    glm::vec3 ydir = normalize( cross( xdir, zdir ) );

    glm::vec3 pos = lookfrom;
    float fov     = vfov;
    float aspect  = aspect_ratio;

    int imgWidth  = width;
    int imgHeight = height;

    glm::vec3 left = normalize( cross( ydir, zdir ) );
    float radfov   = fov * float( M_PI ) / 180.f;
    float h        = tan( radfov * .5f ) * ( 2.0f );
    float w        = h * width / float( height );

    glm::vec3 B                = pos + zdir + ( h / 2.0f * ydir );
    glm::vec3 nearPlaneTopLeft = B + w / 2.0f * left;
    glm::vec3 dXPixel          = -left * ( w / float( width ) );
    glm::vec3 dYPixel          = -ydir * ( w / float( width ) );

    glm::vec3 d =
        glm::normalize( nearPlaneTopLeft + ( 1 + 0.5f ) * dXPixel + ( 1 + .5f ) * dYPixel );
    glm::vec3 ddx =
        ( ( d * d ) * dXPixel - ( d * dXPixel ) * d ) / glm::pow( d * d, glm::vec3( 1.5f ) );
    glm::vec3 ddy =
        ( ( d * d ) * dYPixel - ( d * dYPixel ) * d ) / glm::pow( d * d, glm::vec3( 1.5f ) );

    glm::vec3 d1 =
        glm::normalize( nearPlaneTopLeft + ( 1 + 0.5f ) * dXPixel + ( 2 + .5f ) * dYPixel );

    // Initialise geometric data
    m_color   = glm::vec4( 1.0f, 0.5f, 0.2f, 1.f );
    Vertex v0 = { normalize( glm::vec3( 0.5f, 0.5f, 1.5f ) ), d, glm::vec2( 0.f, 0.f ), m_color };
    Vertex v1 = {
        normalize( glm::vec3( 0.5f, -0.5f, 0.0f ) ), pos, glm::vec2( 0.f, 0.f ), m_color };
    Vertex v2 = { normalize( glm::vec3( -0.5f, -0.5f, 0.0f ) ),
                  d1,
                  glm::vec2( 0.f, 0.f ),
                  glm::vec4( 1, 0, 0, 0 ) };
    Vertex v3 = { normalize( glm::vec3( -0.5f, 0.5f, 1.5f ) ), d, glm::vec2( 0.f, 0.f ), m_color };
    std::vector<Vertex> vertices      = { v0, v1, v2 };
    std::vector<unsigned int> indices = {
        0, 1, 1, 2 // Second Triangle
    };

    m_renderer = new Renderer();

    m_mesh = new Mesh( vertices, indices, GL_TRIANGLES );

    m_programcolor = m_program = new Shader( "Shaders/Basic.vert.glsl", "Shaders/Basic.frag.glsl" );
    m_programnormal = new Shader( "Shaders/Basic.vert.glsl", "Shaders/Normals.frag.glsl" );
}

SimpleTriangle::~SimpleTriangle() {
    delete m_programcolor;
    delete m_programnormal;
    delete m_mesh;
    delete m_renderer;
}

void SimpleTriangle::draw() {
    OpenGLDemo::draw();

    m_renderer->draw( m_mesh, m_program );
}

bool SimpleTriangle::keyboard( unsigned char k ) {
    switch ( k ) {
    case 'c':
        m_program = m_programcolor;
        return true;
    case 'n':
        m_program = m_programnormal;
        return true;
    default:
        return false;
    }
}
