#define STB_IMAGE_IMPLEMENTATION
#include "texture.h"

Texture::Texture(int width, int height, int nrChannels, const GLubyte* data)
{
    glGenTextures(1, &m_texId);
    glBindTexture(GL_TEXTURE_2D, m_texId);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_texId);
}

GLuint Texture::getTextureID() const
{
	return m_texId;
}

int Texture::getWidth()
{
	return width;
}

int Texture::getHeight()
{
	return height;
}