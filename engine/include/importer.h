#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <emscripten.h>

// Format of the .tile Save File

/*
Name of the Map
Map Width
Map Height
TileX
TileY
Clear R
Clear G
Clear B
Grid R
Grid G
Grid B
Amount of Textures
Amount of Layers
Filename
{ENCODED Base64 Image}
Name of Tilelayer
Tilelayer
*/

class Importer
{
public:
    Importer() { }

    void startImport();
    bool saveData();

    std::string getMapName() { return mapName; }
    int getWidth() { return mapWidth; }
    int getHeight() { return mapHeight; }
    int getTileX() { return tileX; }
    int getTileY() { return tileY; }
    float getR() { return clearR; }
    float getG() { return clearG; }
    float getB() { return clearB; }
    float getGridR() { return gridR; }
    float getGridG() { return gridG; }
    float getGridB() { return gridB; }
    float getGridA() { return gridA; }
    std::vector<std::string> getTexNames() { return texNames; }
    std::vector<std::string> getB64Strings() { return b64Strings; }
    std::vector<std::string> getLayerNames() {return layerNames; }
    std::vector<std::vector<std::vector<int>>> getLayers() { return tileLayers; }

private:
    std::string mapName;
    int mapWidth, mapHeight, tileX, tileY;
    float clearR, clearG, clearB, gridR, gridG, gridB, gridA;
    std::vector<std::string> texNames;
    std::vector<std::string> b64Strings;
    std::vector<std::string> layerNames;
    std::vector<std::vector<std::vector<int>>> tileLayers;
};
