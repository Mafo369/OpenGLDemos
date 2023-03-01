#pragma once

class Framebuffer
{
    private:
        unsigned int m_rendererId;

    public:
        Framebuffer();

        ~Framebuffer();

        void bind() const;
        void unbind() const;

        void attachTexture(unsigned int attachment, unsigned int textureId);
        void attachTexture2D(unsigned int attachment, unsigned int textureId);
        void attachCubemapTexture2D(unsigned int attachment, unsigned int face, unsigned int textureId);
        void attachRenderbuffer(unsigned int attachment, unsigned int rboId);
};

