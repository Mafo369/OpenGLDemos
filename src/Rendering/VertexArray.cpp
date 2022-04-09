#include "VertexArray.h"        
#include "Renderer.h"

VertexArray::VertexArray()
{
    glAssert(glGenVertexArrays(1, &m_rendererId));
}

VertexArray::~VertexArray()
{
    glAssert(glDeleteVertexArrays(1, &m_rendererId));
}

void VertexArray::addBuffer(VertexBuffer* vbo, VertexBufferLayout* layout)
{
    bind();
    vbo->bind();
    const auto& elements = layout->getElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++)
    {
        const auto& element = elements[i];
        glAssert(glEnableVertexAttribArray(i));
        glAssert(glVertexAttribPointer(i, element.count, element.type, 
                                       element.normalized, layout->getStride(), 
                                       reinterpret_cast<void*>(offset)));
        offset += element.count * VertexBufferElement::getSizeOfType(element.type);
    }
}

void VertexArray::bind() const
{
    glAssert(glBindVertexArray(m_rendererId));
}

void VertexArray::unbind() const
{
    glAssert(glBindVertexArray(0));
}

template<>
void VertexBufferLayout::push<float>(unsigned int count)
{
   m_elements.push_back({GL_FLOAT, count, GL_FALSE});
   m_stride += count * VertexBufferElement::getSizeOfType(GL_FLOAT);
}

template<>
void VertexBufferLayout::push<unsigned int>(unsigned int count)
{
   m_elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
   m_stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_INT);
}

template<>
void VertexBufferLayout::push<unsigned char>(unsigned int count)
{
   m_elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
   m_stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_BYTE);
}
