#include "Buffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer( const void* data, unsigned int size ) {
    glAssert( glGenBuffers( 1, &m_rendererId ) );
    glAssert( glBindBuffer( GL_ARRAY_BUFFER, m_rendererId ) );
    glAssert( glBufferData( GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW ) );
}

VertexBuffer::~VertexBuffer() {
    glAssert( glDeleteBuffers( 1, &m_rendererId ) );
}

void VertexBuffer::bind() const {
    glAssert( glBindBuffer( GL_ARRAY_BUFFER, m_rendererId ) );
}

void VertexBuffer::unbind() const {
    glAssert( glBindBuffer( GL_ARRAY_BUFFER, 0 ) );
}

IndexBuffer::IndexBuffer( const unsigned int* data, unsigned int count, unsigned int mode ) :
    m_count( count ), m_mode( mode ) {
    glAssert( glGenBuffers( 1, &m_rendererId ) );
    glAssert( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_rendererId ) );
    glAssert(
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, count * sizeof( GLuint ), data, GL_STATIC_DRAW ) );
}

IndexBuffer::~IndexBuffer() {
    glAssert( glDeleteBuffers( 1, &m_rendererId ) );
}

void IndexBuffer::bind() const {
    glAssert( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_rendererId ) );
}

void IndexBuffer::unbind() const {
    glAssert( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ) );
}
