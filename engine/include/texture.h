#pragma once
#include <GLES3/gl3.h>
#include <iostream>
#include "stb_image.h"

class Texture {
public:
    Texture(int width, int height, int nrChannels, const GLubyte* data);
    ~Texture();

    GLuint getTextureID() const;

    int getWidth();
    int getHeight();

private:
    GLuint m_texId;
    int width;
    int height;
};