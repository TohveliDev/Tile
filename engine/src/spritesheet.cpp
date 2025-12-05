#include "spritesheet.h"

//Sprite sheet:
SpriteSheet::SpriteSheet(std::shared_ptr<Texture> texPtr, const glm::ivec2& size, const glm::ivec2& singleTexSize, const glm::ivec2& padding) {
    //the actual OpenGL texture we're using
    m_textureAtlas = texPtr;
    //the size of the Sprite sheet
    m_sheetSize = size;
    //if there's padding:
    m_texturePadding = padding;
    //size of a single texture in the sprite sheet: 
    //assume that all sprites in the sheet are the same size
    m_singleSize = singleTexSize;
    //save the last tex index we allow for this spritesheet
    m_lastSpriteIndex = calcLastIndex();
}

SpriteSheet::SpriteSheet() {
    m_textureAtlas = 0;
    m_sheetSize = glm::ivec2(0);
    m_texturePadding = glm::ivec2(0);
    m_singleSize = glm::ivec2(0);
    m_lastSpriteIndex = 0;
}

SpriteSheet::~SpriteSheet() {
    m_textureAtlas = 0;
}

int SpriteSheet::calcLastIndex() {
    int i = 0;
    for(size_t x = 0; x < m_sheetSize.x; x++) {
        for(size_t y = 0; y < m_sheetSize.y; y++) {
            i++;
        }
    }
    return i;
}

int SpriteSheet::getSpriteIndex(const glm::ivec2& spriteCoord) {
    int i = 0;
    //bounds check
    if(!(spriteCoord.x < m_sheetSize.x && spriteCoord.y < m_sheetSize.y)) {
        return -1;
    }

    for(size_t x = 0; x < m_sheetSize.x; x++) {
        for(size_t y = 0; y < m_sheetSize.y; y++) {
            i++;
            if(x == spriteCoord.x && y == spriteCoord.y) {
                return i-1;
            }
        }
    }

    return -1;
}

glm::ivec2 SpriteSheet::getSpriteTexCoord(int spriteIndex) {
    if(spriteIndex > m_lastSpriteIndex) {
        return glm::ivec2(-1);
    }

    int i = 0;
    for(size_t x = 0; x < m_sheetSize.x; x++) {
        for(size_t y = 0; y < m_sheetSize.y; y++) {
            if(i == spriteIndex) {
                glm::ivec2 texCoordOrigin = glm::ivec2(0);
                texCoordOrigin.x = x * (m_singleSize.x + m_texturePadding.x);
                texCoordOrigin.y = y * (m_singleSize.y + m_texturePadding.y);                

                return texCoordOrigin;
            }
            i++;
        }
    }
}
//End - Sprite sheet