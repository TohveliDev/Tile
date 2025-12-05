#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "renderer.h"

struct tileMap {
    typedef std::vector<std::vector<int>> tileLayer;

    tileLayer                 m_mapLayer;
    std::vector<std::string>  m_imageFileNames;
    glm::ivec2                m_mapSize;
    glm::ivec2                m_tileSize;
    std::string               m_mapName;

    void renderMap(Renderer* renderer) {
      for (int i = 0; i < m_mapLayer.size(); i++)
      {
        for (int j = 0; j < m_mapLayer[i].size(); j++)
        {
          glm::ivec2 position = { j, i };
          int texID = m_mapLayer[i][j];
          renderer->drawTile(position, m_tileSize, texID, i);
        }
      }
    }
  };

struct layeredTileMap {
  using tileRow = std::vector<int>;
  using tileLayer = std::vector<tileRow>;
  using tileMap = std::vector<tileLayer>;

  tileMap                   m_mapLayers;
  std::vector<std::string>  m_imageFileNames;
  glm::ivec2                m_mapSize;
  glm::ivec2                m_tileSize = {64, 64};
  std::string               m_mapName;
  std::vector<std::string>  m_layerNames;
  std::vector<glm::vec4>    m_layerTintColors;

  void renderMap(Renderer* renderer) {
    //each layer
    for (auto it = m_mapLayers.rbegin(); it != m_mapLayers.rend(); it++) {
      int i = std::distance(it, m_mapLayers.rend()) - 1;
      //each column
      for(int j = 0; j < m_mapLayers[i].size(); j++) {
        //each row
        for(int k = 0; k < m_mapLayers[i][j].size(); k++) {
          glm::ivec2 position = {k, j};
          int texID = m_mapLayers[i][j][k];
          renderer->drawTile(position, m_tileSize, texID, m_mapLayers.size()-(i+1));
        }
      }
    }

    renderer->setMaxLayers(m_mapLayers.size());
  }

  void initEmptyMap(int layers, int rows, int cols) {
    tileRow emptyRow(cols, -1);
    tileLayer emptyLayer(rows, emptyRow);
    tileMap emptyMap(layers, emptyLayer);
    m_mapSize.x = cols;
    m_mapSize.y = rows;
    m_mapLayers = emptyMap;
  }

  void clearMap() {
    tileRow emptyRow(m_mapSize.x, -1);
    tileLayer emptyLayer(m_mapSize.y, emptyRow);
    tileMap emptyMap(m_mapLayers.size(), emptyLayer);
    m_mapLayers = emptyMap;
    m_layerNames.clear();
  }

  void removeTextureFromMap(int texId) {
    //each layer
    for(int i = 0; i < m_mapLayers.size(); i++) {
      //each column
      for(int j = 0; j < m_mapLayers[i].size(); j++) {
        //each row
        for(int k = 0; k < m_mapLayers[i][j].size(); k++) {
          if(m_mapLayers[i][j][k] == texId) {
            m_mapLayers[i][j][k] = -1;
          }
          if(m_mapLayers[i][j][k] > texId) {
            m_mapLayers[i][j][k] -= 1;
          }
        }
      }
    }
  }

  int getTileId(int layer, int xCoord, int yCoord) {
    if(layer < 0 || layer > m_mapLayers.size() || xCoord < 0 || xCoord > m_mapLayers[0][0].size() || yCoord < 0 || yCoord > m_mapLayers[0].size()) {
      //printf("Ey, yo! out of bounds.");
      return -1;
    }
    return m_mapLayers[layer][yCoord][xCoord];
  }

  void setTileId(int layer, int xCoord, int yCoord, int newValue) {
    if(layer < 0 || xCoord < 0 || yCoord < 0) {
      //printf("Ey, yo! out of bounds.");
      return;
    }
    else {
      m_mapLayers[layer][yCoord][xCoord] = newValue;
    }
  }

  void drawGrid(Renderer* renderer, glm::vec4 gridColor) {
    int layers = m_mapLayers.size();
    int rows = m_mapSize.y;
    int cols = m_mapSize.x;
    glm::vec2 start = {0, 0};
    glm::vec2 end = {0, 0};

    for(int i = 0; i <= cols; i++) {
      start = {i * m_tileSize.x, 0.f};
      end = {i * m_tileSize.x, rows* m_tileSize.y};
      renderer->drawLine(start, end, gridColor);
    }

    for(int j = 0; j <= rows; j++) {
      start = {0.f, j * m_tileSize.y};
      end = {cols * m_tileSize.x, j * m_tileSize.y};
      renderer->drawLine(start, end, gridColor);
    }
  }
};