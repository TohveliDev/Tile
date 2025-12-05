#pragma once
#include <string>
#include <vector>
#include <ranges>
#include <fstream>
#include <sstream>
#include <memory>
#include <glm/glm.hpp>
#include <GLFW/emscripten_glfw3.h>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <GLES3/gl3.h>
#include <emscripten/html5.h>
#include "encoder.h"

#ifndef EXPORTER_H
#define EXPORTER_H

#ifdef __cplusplus
extern "C" {
#endif

void downloadFile(const char* filename, const char* text);

#ifdef __cplusplus
}
#endif // END extern

#endif // END EXPORTER_H

class Exporter
{
public:
    Exporter();

    void exportToTxt(std::string mapName, std::vector<std::vector<std::vector<int>>> tileMap, std::vector<std::string> layerNames, 
        std::vector<std::string> imageNames, glm::ivec2 tileSize, glm::vec3 RGB);

    void exportToHFile(std::string mapName, std::vector<std::vector<std::vector<int>>> tileMap, std::vector<std::string> layerNames, 
        std::vector<std::string> imageNames, glm::ivec2 tileSize, glm::vec3 RGB);

    void exportToJSON(std::string mapName, std::vector<std::vector<std::vector<int>>> tileMap, std::vector<std::string> layerNames, glm::ivec2 tileSize);

    void saveMap(std::string mapName, std::vector<std::string> fileNames, glm::ivec2 tileSize, glm::vec3 RGB,
        std::vector<std::vector<std::vector<int>>> tileMap, std::vector<std::string> layerNames, glm::vec4 gridRGBA);
private:
    std::unique_ptr<Encoder> m_encoder;
};