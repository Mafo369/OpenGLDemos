#ifndef BUFFER_H
#define BUFFER_H
#pragma once

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
        unsigned int m_mode;

    public:
        IndexBuffer( const unsigned int* data, unsigned int count, unsigned int mode );

        ~IndexBuffer();

        void bind() const;
        void unbind() const;

        inline unsigned int getCount() const { return m_count; }
        inline unsigned int getMode() const { return m_mode; }
};
#endif
