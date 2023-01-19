#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H
#pragma once

#include "Buffer.h"
#include <cassert>
#include <iostream>

// Hard coded to avoid cyclical includes
#define GL_FLOAT				0x1406
#define GL_UNSIGNED_INT				0x1405
#define GL_UNSIGNED_BYTE			0x1401

struct VertexBufferElement
{
   unsigned int type;
   unsigned int count;
   unsigned char normalized;

   static unsigned int getSizeOfType(unsigned int type){
       switch (type)
       {
           case GL_FLOAT: return 4;
           case GL_UNSIGNED_INT: return 4;
           case GL_UNSIGNED_BYTE: return 1;
       }
       assert(false);
       return 0;
   }
};

class VertexBufferLayout
{
    private:
        std::vector<VertexBufferElement> m_elements;
        unsigned int m_stride;
    public:
        VertexBufferLayout() : m_stride(0) {}

        template<typename T> void push(unsigned int count);

        inline const std::vector<VertexBufferElement> getElements() const { return m_elements; }
        inline unsigned int getStride() const { return m_stride; }
};

class VertexArray
{
    private:
        unsigned int m_rendererId;
    public:
        VertexArray();

        ~VertexArray();

        void addBuffer(VertexBuffer* vbo, VertexBufferLayout* layout);

        void bind() const;
        void unbind() const;
};

#endif
