#pragma once
#include "texture.h"
#include "spritesheet.h"
#include <glm/glm.hpp>

// class Sprite {
// public:
//     Sprite(std::shared_ptr<Texture> tex, glm::ivec2 size, bool useAtlas = false, glm::ivec2 atlasCoords = glm::ivec2(0), SpriteSheet sSheet = SpriteSheet());
//     ~Sprite();
//     int getIndex() { return index; }
//     //render()?

// private:
//     std::shared_ptr<Texture>      m_texture;
//     glm::ivec2                    m_size;
//     glm::ivec2                    m_atlasCoords;
//     int                           index;
//     bool                          usingAtlas;
// };

struct Sprite {
    std::shared_ptr<Texture> m_texture; // tekstuuri,
    glm::vec2 m_pos;                    // paikka (world pos)
    glm::ivec2 m_size;                  // piirrettävän Spriten koko,
    glm::ivec2 m_atlasCoords;           // atlas/spritesheet koordinaatti
    glm::vec4 m_color;
    int m_index;                          // indeksi
    int m_zLayer;
    bool usingSpriteSheet;              // käyttääkö atlasta/spriteSheettiä

    //full texture
    Sprite(std::shared_ptr<Texture> tex, glm::ivec2 size, int texVecIndex) {
        m_texture = tex;
        m_size = size;
        m_atlasCoords = glm::ivec2(0);
        usingSpriteSheet = false;
        m_index = texVecIndex;
    }

    //partial texture//atlas
    Sprite(std::shared_ptr<Texture> tex, glm::ivec2 size, bool useSpriteSheet, SpriteSheet sSheet, glm::ivec2 atlasCoords) {
        m_texture = tex;
        m_size = size;
        m_atlasCoords = atlasCoords;
        usingSpriteSheet = useSpriteSheet;

        m_index = sSheet.getSpriteIndex(atlasCoords);
    }
};


// void Renderer::drawSpriteFromSheet(const Sprite& sprite) {
//     if(sprite.m_usingSpriteSheet) {
//         return;
//     }

//     sprite.m_pos;
//     texId = sprite.m_texture->getTextureID();
//     if(texId != -1) {

//         sb->addSpriteFromAtlas(
//             texId, 
//             sprite.m_pos, 
//             sprite.m_size, 
//             sprite.m_zLayer, 
//             glm::vec2(sprite.m_atlasCoords.x, sprite.m_atlasCoords.y), 
//             glm::vec2(sprite.m_size.x, sprite.m_size.y), 
//             sprite.m_color);
//     }
// }