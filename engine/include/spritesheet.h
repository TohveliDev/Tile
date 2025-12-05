#pragma once
#include <glm/glm.hpp>
#include "texture.h"

class SpriteSheet {
    public:
        SpriteSheet(std::shared_ptr<Texture> texPtr,
                    const glm::ivec2& size,
                    const glm::ivec2& singleTexSize,
                    const glm::ivec2& padding);
        SpriteSheet();
        ~SpriteSheet();

        //Texture stuff
        GLuint  getTextureID() const { return m_textureAtlas->getTextureID(); }
        std::shared_ptr<Texture> getTexPtr() { return m_textureAtlas; }
        int     getTextureWidth() { return m_textureAtlas->getWidth(); }
        int     getTextureHeight() { return m_textureAtlas->getHeight(); }

        //Counting from bottom-left corner
        int         calcLastIndex();
        int         getSpriteIndex(const glm::ivec2& spriteCoord);
        glm::ivec2  getSpriteTexCoord(int spriteIndex);

    private:
        std::shared_ptr<Texture> m_textureAtlas;
        glm::ivec2               m_sheetSize;
        glm::ivec2               m_texturePadding;
        glm::ivec2               m_singleSize;
        int                      m_lastSpriteIndex;
};