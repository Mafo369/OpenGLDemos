#pragma once

#include "Renderer.h"
#include <vector>

class VertexBuffer
{
    private:
        unsigned int m_rendererId;

    public:
        VertexBuffer( const void* data, unsigned int size );

        ~VertexBuffer();

        void bind() const;
        void unbind() const;
};

class IndexBuffer
{
    private:
        unsigned int m_rendererId;
        unsigned int m_count;

    public:
        IndexBuffer( const unsigned int* data, unsigned int count );

        ~IndexBuffer();

        void bind() const;
        void unbind() const;

        inline unsigned int getCount() const { return m_count; }
};
