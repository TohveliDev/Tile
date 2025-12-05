#include "spritebatch.h"

SpriteBatch::SpriteBatch() {
  init();
}

SpriteBatch::~SpriteBatch() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_lineVbo);

    glDeleteVertexArrays(1, &m_vao);
    glDeleteVertexArrays(1, &m_lineVao);
}

void SpriteBatch::init() {
    //gen and bind vertex attribute object
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // gen and bind vertex buffer object
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_DYNAMIC_DRAW);

    //stride is 9*float, 'cause of total vertex data being 3*float for pos, 2*float for texCoords, 4*float for colorData
    glVertexAttribPointer(0 , 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //AttribPointer for color data, maybe not required?
    //offset of 5*float, 'cause of position and texCoords coming first
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //init vao and vbo for drawing lines
    glGenVertexArrays(1, &m_lineVao);
    glBindVertexArray(m_lineVao);

    glGenBuffers(1, &m_lineVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVbo);
    glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_DYNAMIC_DRAW);

    //attrib pointers for line position data (and color)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //unbind vbo and vao to avoid complications
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SpriteBatch::clear() {
    //empty all data. inefficient for now but it works :D
    m_sprites.clear();
    m_lineData.clear();
    m_vertexData.clear();
}

//Add a sprite to the batch, pos being left bottom corner position
void SpriteBatch::addSprite(GLuint texId, const glm::vec2& pos, const glm::vec2& size, const int& zLayer, const glm::vec4& color) {
    //add sprite data to an array/vector

    //add texId only if unique:
    if(std::find(m_uniqueTexIds.begin(), m_uniqueTexIds.end(), texId) == m_uniqueTexIds.end()) {
        m_uniqueTexIds.push_back(texId);
    }

    SpriteBatch::SpriteData sd;

    //position (3 floats), texCoords (2 floats), color (4 floats)
    float vertexData [] = {
        pos.x         , pos.y,          0.0f, 0.0f, 1.0f, color.r, color.g, color.b, color.a, //lower left
        pos.x + size.x, pos.y,          0.0f, 1.0f, 1.0f, color.r, color.g, color.b, color.a, //lower right
        pos.x + size.x, pos.y + size.y, 0.0f, 1.0f, 0.0f, color.r, color.g, color.b, color.a, //upper right
        pos.x,          pos.y,          0.0f, 0.0f, 1.0f, color.r, color.g, color.b, color.a, //lower left
        pos.x + size.x, pos.y + size.y, 0.0f, 1.0f, 0.0f, color.r, color.g, color.b, color.a, //upper right
        pos.x,          pos.y + size.y, 0.0f, 0.0f, 0.0f, color.r, color.g, color.b, color.a  //upper left
    };

    for(float value : vertexData) {
        sd.vertexData.push_back(value);
    }

    sd.texId = texId;
    sd.zLayer = zLayer;
    m_sprites.push_back(sd);
}

void SpriteBatch::addSpriteFromAtlas(GLuint atlasTexId, const glm::vec2& pos, const glm::vec2& size, const int& zLayer, const glm::vec2& atlasCoords, const glm::vec2& texSize, const glm::vec4& color) {
    //add sprite data to an array/vector from a texture Atlas
    //check if the atlas is in use:
    if(std::find(m_uniqueTexIds.begin(), m_uniqueTexIds.end(), atlasTexId) == m_uniqueTexIds.end()) {
        m_uniqueTexIds.push_back(atlasTexId);
    }

    SpriteBatch::SpriteData sd;

    //position (3 floats), texCoords (2 floats), color (4 floats)
    float vertexData [] = {
        pos.x,          pos.y,          0.0f, atlasCoords.x * texSize.x,               (atlasCoords.y * texSize.y) + texSize.y, color.r, color.g, color.b, color.a, //lower left
        pos.x + size.x, pos.y,          0.0f, (atlasCoords.x * texSize.x) + texSize.x, (atlasCoords.y * texSize.y) + texSize.y, color.r, color.g, color.b, color.a, //lower right
        pos.x + size.x, pos.y + size.y, 0.0f, (atlasCoords.x * texSize.x) + texSize.x, atlasCoords.y * texSize.y,               color.r, color.g, color.b, color.a, //upper right
        pos.x,          pos.y,          0.0f, atlasCoords.x * texSize.x,               (atlasCoords.y * texSize.y) + texSize.y, color.r, color.g, color.b, color.a, //lower left
        pos.x + size.x, pos.y + size.y, 0.0f, (atlasCoords.x * texSize.x) + texSize.x, atlasCoords.y * texSize.y,               color.r, color.g, color.b, color.a, //upper right
        pos.x,          pos.y + size.y, 0.0f, atlasCoords.x * texSize.x,               atlasCoords.y * texSize.y,               color.r, color.g, color.b, color.a  //upper left
    };

    for(float value : vertexData) {
        sd.vertexData.push_back(value);
    }

    sd.texId = atlasTexId;
    sd.zLayer = zLayer;
    m_sprites.push_back(sd);
}

void SpriteBatch::addLine(const glm::vec2& startPos, const glm::vec2& endPos, const glm::vec4& color) {
    //add line data to an array/vector

    m_lineData.push_back(startPos.x);
    m_lineData.push_back(startPos.y);
    m_lineData.push_back(endPos.x);
    m_lineData.push_back(endPos.y);
}

void SpriteBatch::render(Shader* shader, Camera* camera) {
    //don't render if no sprites
    if(!m_sprites.empty()) { 

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindVertexArray(m_vao);
        for(int i = 0; i < maxLayers; i++) {
            //From each texture
            for(GLuint& uTexId : m_uniqueTexIds) {
                m_vertexData.clear();
                //each sprite
                for(auto& spriteData : m_sprites) {
                    //if same texId
                    if(spriteData.texId == uTexId) {
                        //push to queue
                        for(auto& data : spriteData.vertexData) {
                            if(spriteData.zLayer == i) { m_vertexData.push_back(data); }
                            else { continue; }
                        }
                    }
                }


                shader->bind();
                shader->setUniformMat4("u_mvpMatrix", camera->getProjectionMatrix() * camera->getViewMatrix() /* getModelMatrix()*/);
                shader->setUniform1i("usingSampler", 1);

	            glActiveTexture(GL_TEXTURE0);
	            glBindTexture(GL_TEXTURE_2D, uTexId);
                shader->setUniform1i("uTexture", 0);

                glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
                glBufferData(GL_ARRAY_BUFFER, m_vertexData.size() * sizeof(float), m_vertexData.data(), GL_DYNAMIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindVertexArray(m_vao);
                glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertexData.size() / 9));

                glBindTexture(GL_TEXTURE_2D, 0);

                glBindVertexArray(0);
            } // END - uniqueTex
        } // END - layers
    } // END - sprites

    if(m_lineData.empty()) {
         return;
    }

    glBindVertexArray(m_lineVao);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVbo);
    glBufferData(GL_ARRAY_BUFFER, m_lineData.size() * sizeof(float), m_lineData.data(), GL_DYNAMIC_DRAW);
    glLineWidth(2.0f); //ignored by some drivers and always sets linewidth to 1

    shader->bind();
    shader->setUniform1i("usingSampler", 0);
    shader->setUniform4f("u_color", lineColor);
    shader->setUniformMat4("u_mvpMatrix", camera->getProjectionMatrix() * camera->getViewMatrix());
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_lineData.size() / 2));
    glBindVertexArray(0);

    clear();
}

