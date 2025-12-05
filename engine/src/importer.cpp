#include "importer.h"

// Because Emscripten needs to use global functions, the parsing is done globally
// and the data is put into global variables.

// BEGIN - Parsing
bool isParsed = false;
std::string t_mapName;
int t_mapWidth, t_mapHeight, t_tileX, t_tileY;
float t_clearR, t_clearG, t_clearB, t_gridR, t_gridG, t_gridB, t_gridA;
std::vector<std::string> t_texNames;
std::vector<std::string> t_b64Strings;
std::vector<std::string> t_layerNames;
std::vector<std::vector<std::vector<int>>> t_mapLayers;

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void dataToTileLayer(const std::string& line, int width, int height)
    {
        std::stringstream ss(line);

        std::vector<int> flat;
        int val;

        while (ss >> val) 
        {
            flat.push_back(val);
        }

        if (flat.size() != width * height)
        {
            return;
        }

        std::vector<std::vector<int>> tileLayer(height, std::vector<int>(width));

        int index = 0;
        for (int i = 0; i < height; i++)
            for (int j = 0; j < width; j++)
                tileLayer[i][j] = flat[index++];

        t_mapLayers.push_back(std::move(tileLayer));
    }
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void parseTileFile(const char* data)
    {
        // Befpre parsing, we clear all previous data
        t_mapName = "";
        t_mapWidth = 0;
        t_mapHeight = 0;
        t_tileX = 0;
        t_tileY = 0;
        t_clearR = 0.f;
        t_clearG = 0.f;
        t_clearB = 0.f;
        t_gridR = 0.f;
        t_gridG = 0.f;
        t_gridB = 0.f;
        t_texNames.clear();
        t_b64Strings.clear();
        t_layerNames.clear();
        t_mapLayers.clear();

        std::string text(data);
        std::stringstream ss(text);

        std::string line;
        int texCount;
        int layerCount;

        // 1. Parse the Fixed Data
        std::getline(ss, t_mapName);                                        // Row 1  - Map Name
        std::getline(ss, line); t_mapWidth = std::stoi(line);               // Row 2 -  Map Width
        std::getline(ss, line); t_mapHeight = std::stoi(line);              // Row 3 - Map Height
        std::getline(ss, line); t_tileX = std::stoi(line);                  // Row 4 - Tile Width
        std::getline(ss, line); t_tileY = std::stoi(line);                  // Row 5 - Tile Height
        std::getline(ss, line); t_clearR = std::stof(line);                 // Row 6 - R (Clear Color)
        std::getline(ss, line); t_clearG = std::stof(line);                 // Row 7 - G (Clear Color)
        std::getline(ss, line); t_clearB = std::stof(line);                 // Row 8 - B (Clear Color)
        std::getline(ss, line); t_gridR = std::stof(line);                  // Row 9 - R (Grid Color)
        std::getline(ss, line); t_gridG = std::stof(line);                  // Row 10 - G (Grid Color)
        std::getline(ss, line); t_gridB = std::stof(line);                  // Row 11 - B (Grid Color)
        std::getline(ss, line); t_gridA = std::stof(line);                  // Row 12 - B (Grid Color)
        std::getline(ss, line); texCount = std::stoi(line);                 // Row 13 - Amount of Loaded Textures
        std::getline(ss, line); layerCount = std::stoi(line);               // Row 14 - Amount of Tile Layers

        // 2. Parse the Textures
        const char* ptr = data;
        const char* end = data + strlen(data);

        int skippedLines = 14;

        for (int j = 0; j < skippedLines; j++)
        {
            while (ptr < end && *ptr != '\n') ++ptr;
            if (ptr < end && *ptr == '\n') ++ptr;
        }

        auto readPtr = [&](std::string& out)
        {
            const char* start = ptr;
            while (ptr < end && *ptr != '\n') ++ptr;
            out.assign(start, ptr - start);
            if (ptr < end && *ptr == '\n') ++ptr;
        };

        for (int i = 0; i < texCount; i++)
        {
            std::string texName;
            readPtr(texName);

            std::string b64Image;
            readPtr(b64Image);

            t_texNames.push_back(texName);
            t_b64Strings.push_back(std::move(b64Image));
        }

        // 3. Parse the Map Layers
        for (int j = 0; j < layerCount; j++)
        {
            std::string layerName;
            readPtr(layerName);

            std::string flatLayer;
            readPtr(flatLayer);

            t_layerNames.push_back(layerName);
            dataToTileLayer(flatLayer, t_mapWidth, t_mapHeight);
        }

        isParsed = true; // Tells the program that the data is parsed
    }
}

EM_JS(void, startImportProcess, (), {
    openDialog();
})
// END - Parsing

void Importer::startImport()
{
    startImportProcess();
}

bool Importer::saveData()
{
    if (!isParsed) return false; // Parsing not finished

    // Copy parsed data into importer instance
    mapName = t_mapName;
    mapWidth = t_mapWidth;
    mapHeight = t_mapHeight;
    tileX = t_tileX;
    tileY = t_tileY;
    clearR = t_clearR;
    clearG = t_clearG;
    clearB = t_clearB;
    gridR = t_gridR;
    gridG = t_gridG;
    gridB = t_gridB;
    gridA = t_gridA;
    texNames = t_texNames;
    b64Strings = t_b64Strings;
    layerNames = t_layerNames;
    tileLayers = t_mapLayers;

    isParsed = false;
    return true;
}