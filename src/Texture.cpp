#include "Texture.h"
#include "Renderer.h"
#include "error_handling.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb_image.h"
#include <GL/gl.h>

Texture::Texture(const std::string& path): m_rendererId(0), m_filePath(path), 
    m_localBuffer(nullptr), m_width(0), m_height(0), m_bpp(0) {
    stbi_set_flip_vertically_on_load(1);
    m_localBuffer = stbi_load(path.c_str(), &m_width, &m_height, &m_bpp, 4);

    glAssert(glGenTextures(1, &m_rendererId));
    glAssert(glBindTexture(GL_TEXTURE_2D, m_rendererId));
    
    glAssert(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    glAssert(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    glAssert(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    glAssert(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    glAssert(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_localBuffer));
    glAssert(glGenerateMipmap(GL_TEXTURE_2D));
    glAssert(glBindTexture(GL_TEXTURE_2D, 0));

    if(m_localBuffer)
        stbi_image_free(m_localBuffer);
}

Texture::~Texture(){
    glAssert(glDeleteTextures(1, &m_rendererId));
}

void Texture::bind(unsigned int slot) const{
    glAssert(glActiveTexture(GL_TEXTURE0));
    glAssert(glBindTexture(GL_TEXTURE_2D, m_rendererId));
}

void Texture::unbind(){
    glAssert(glBindTexture(GL_TEXTURE_2D, 0));
}
