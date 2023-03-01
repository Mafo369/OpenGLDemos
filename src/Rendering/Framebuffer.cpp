#include "Framebuffer.h"
#include "Renderer.h"

Framebuffer::Framebuffer() {
    glGenFramebuffers(1, &m_rendererId);
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &m_rendererId);
}

void Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_rendererId);
}

void Framebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 1);
}

void Framebuffer::attachTexture(unsigned int attachment, unsigned int textureId) {
    glFramebufferTexture(GL_FRAMEBUFFER, attachment, textureId, 0);
}

void Framebuffer::attachTexture2D(unsigned int attachment, unsigned int textureId) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, textureId, 0);
}

void Framebuffer::attachRenderbuffer(unsigned int attachment, unsigned int rboId) {
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rboId);   
}

void Framebuffer::attachCubemapTexture2D(unsigned int attachment, unsigned int face, unsigned int textureId) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, face, textureId, 0);
}
