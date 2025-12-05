#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <glm/glm.hpp>
#include "shader.h"
#include "camera.h"

//fwd decl
//class Renderer; //Not needed? maybe?
class Shader;
class Camera;

class SpriteBatch {
public:
  SpriteBatch();
  ~SpriteBatch();

  void render(Shader* shader, Camera* camera);
  void clear();

  void addSprite(GLuint texId, const glm::vec2& pos, const glm::vec2& size, const int& zLayer, const glm::vec4& color = glm::vec4(1.0f));
  void addSpriteFromAtlas(GLuint atlasTexId, const glm::vec2& pos, const glm::vec2& size, const int& zLayer, const glm::vec2& atlasCoords, const glm::vec2& texSize, const glm::vec4& color = glm::vec4(1.0f));
  void addLine(const glm::vec2& startPos, const glm::vec2& endPos, const glm::vec4& color = glm::vec4(1.0f));
  void setMaxLayers(int z) { maxLayers = z; }
  void setLineColor(const glm::vec4& newColor) { lineColor = newColor;}
  //void addLayerTint();

private:
  struct SpriteData {
    GLuint texId;
    int zLayer;
    std::vector<float> vertexData;
  };

  void init();

  GLuint m_vao;
  GLuint m_vbo;
  GLuint m_lineVao;
  GLuint m_lineVbo;

  bool usingColorData = false;
  glm::vec4 lineColor = glm::vec4(1.0f);

  std::vector<SpriteData> m_sprites;
  std::vector<float> m_lineData;
  std::vector<float> m_vertexData;
  std::vector<GLuint> m_uniqueTexIds;
  int maxLayers;
};