#pragma once

#include <string>
#include <glm/glm.hpp>

class Texture {
public:
    Texture() {};
    Texture(const std::string& path);
    Texture(const glm::vec3& color);
    ~Texture();

    void bind(unsigned int slot = 0) const;
    void unbind();

    inline int getWidth() const { return m_width; }
    inline int getHeight() const { return m_height; }

    unsigned int m_rendererId;
    std::string m_filePath, type;
    unsigned char* m_localBuffer;
    int m_width, m_height, m_bpp;
private:
};
