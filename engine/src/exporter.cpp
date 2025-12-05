#include "exporter.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

EM_JS(void, downloadMap, (const char* filename, const char* text), {
    var name = UTF8ToString(filename);
    var content = UTF8ToString(text);

    var blob = new Blob([content], {type: "text/plain"});
    var link = document.createElement("a");
    link.href = window.URL.createObjectURL(blob);
    link.download = name;
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
});

Exporter::Exporter()
{
    m_encoder = std::make_unique<Encoder>();
}

using namespace std::ranges;
void Exporter::exportToTxt(std::string mapName, std::vector<std::vector<std::vector<int>>> tileMap, 
    std::vector<std::string> layerNames, std::vector<std::string> imageNames, glm::ivec2 tileSize, glm::vec3 RGB)
{
    std::ostringstream oss;
    std::string fileName = mapName + ".txt";

    oss << "float tileSizeX = " << tileSize.x << ".0f;\n";
    oss << "float tileSizeY = " << tileSize.y << ".0f;\n\n";

    oss << "For SFML: use the ints provided as the clear color\n";
    oss << "int sfmlR = " << static_cast<int>(ceil(255 * RGB[0])) << ";\n";
    oss << "int sfmlG = " << static_cast<int>(ceil(255 * RGB[1])) << ";\n";
    oss << "int sfmlB = " << static_cast<int>(ceil(255 * RGB[2])) << ";\n\n";

    oss << "For OpenGL: use the floats provided as the clear color\n";
    oss << "float glR = " << RGB[0] << ";\n";
    oss << "float glG = " << RGB[1] << ";\n";
    oss << "float glB = " << RGB[2] << ";\n\n";

    oss << "static const std::vector<std::string> fileNames = {\n";

    for (const auto& name : imageNames)
    {
        oss << "    " << std::quoted(name) << ",\n";
    }

    oss << "}; \n\n";

    for (int i = 0; i < tileMap.size(); i++)
    {
        std::vector<std::vector<int>> layer = tileMap[i];
        std::string layerName = layerNames[i];
        layerName.erase(std::remove(layerName.begin(), layerName.end(), ' '), layerName.end());
        if (layerName.empty()) { layerName = "/*Name Your Layer!*/"; }

        oss << "std::vector<std::vector<int>> " << layerName << " = {\n";

        for (const auto& row : views::reverse(layer))
        {
            oss << "    {";
            std::streampos rowStart = oss.tellp();

            for (int tile : row) 
            {
                oss << tile << ", ";
            }

            std::string tempString = oss.str();
            tempString.erase(tempString.size()-2, 2);

            oss.str(tempString);
            oss.seekp(0, std::ios::end);
            
            oss << "},\n";
        }
        oss << "};\n\n";
    }

    std::string mapString = oss.str();
    downloadMap(fileName.c_str(), mapString.c_str());
}

using namespace std::ranges;
void Exporter::exportToHFile(std::string mapName, std::vector<std::vector<std::vector<int>>> tileMap, 
    std::vector<std::string> layerNames, std::vector<std::string> imageNames, glm::ivec2 tileSize, glm::vec3 RGB)
{
    std::ostringstream oss;
    std::string fileName = mapName + ".h";

    oss << "#pragma once\n";
    oss << "#include <string>\n";
    oss << "#include <vector>\n\n";

    oss << "/* To use this map in your project, just add \n";
    oss << "#include \"" << fileName << ".h\"" << "\n";
    oss << "to the top of your main.cpp file! */\n\n";

    oss << "struct tileMap {\n";

    oss << "    float tileSizeX = " << tileSize.x << ".0f;\n";
    oss << "    float tileSizeY = " << tileSize.y << ".0f;\n";

    oss << "    // For SFML: use the ints provided as the clear color\n";
    oss << "    int sfmlR = " << static_cast<int>(ceil(255 * RGB[0])) << ";\n";
    oss << "    int sfmlG = " << static_cast<int>(ceil(255 * RGB[1])) << ";\n";
    oss << "    int sfmlB = " << static_cast<int>(ceil(255 * RGB[2])) << ";\n\n";

    oss << "    // For OpenGL: use the floats provided as the clear color\n";
    oss << "    float glR = " << RGB[0] << ";\n";
    oss << "    float glG = " << RGB[1] << ";\n";
    oss << "    float glB = " << RGB[2] << ";\n\n";

    oss << "    const std::vector<std::string> fileNames = {\n";

    for (const auto& name : imageNames)
    {
        oss << "        " << std::quoted(name) << ",\n";
    }

    oss << "    }; \n\n";

    for (int i = 0; i < tileMap.size(); i++)
    {
        std::vector<std::vector<int>> layer = tileMap[i];
        std::string layerName = layerNames[i];
        layerName.erase(std::remove(layerName.begin(), layerName.end(), ' '), layerName.end());
        if (layerName.empty()) { layerName = "/*Name Your Layer!*/"; }
        oss << "    std::vector<std::vector<int>> " << layerName << " = {\n";

        for (const auto& row : views::reverse(layer))
        {
            oss << "        {";
            std::streampos rowStart = oss.tellp();

            for (int tile : row)
            {
                oss << tile << ", ";
            }

            std::string tempString = oss.str();
            tempString.erase(tempString.size()-2, 2);

            oss.str(tempString);
            oss.seekp(0, std::ios::end);
            
            oss << "},\n";
        }

        oss << "    };\n";
    }

    oss << "};";
    std::string mapString = oss.str();
    downloadMap(fileName.c_str(), mapString.c_str());
}

using namespace std::ranges;
void Exporter::exportToJSON(std::string mapName, std::vector<std::vector<std::vector<int>>> tileMap, std::vector<std::string> layerNames, glm::ivec2 tileSize)
{
    std::ostringstream oss;
    std::string fileName = mapName;
    oss << "{\n";
    oss << "  \"type\": \"map\", \n";
    oss << "  \"orientation\": \"orhogonal\", \n";
    oss << "  \"tilewidth\": " << tileSize.x << ",\n";
    oss << "  \"tileheight\": " << tileSize.y << ",\n";
    oss << "  \"width\": " << tileMap[0][0].size() << ",\n";
    oss << "  \"height\": " << tileMap[0].size() << ",\n";
    oss << "  \"infinite\": false, \n\n";

    oss << "  \"layers\": [\n";

    for (int i = 0; i < tileMap.size(); i++)
    {
        std::vector<std::vector<int>> layer = tileMap[i];
        oss << "    {\n";
        oss << "      \"id\": " << i+1 << ",\n";
        oss << "      \"name\": " << std::quoted(layerNames[i]) << ",\n";
        oss << "      \"visible\": true, \n";
        oss << "      \"opacity\": 1, \n";
        oss << "      \"width\": " << tileMap[i][0].size() << ",\n";
        oss << "      \"height\": " << layer.size() << ",\n";
        oss << "      \"x\": 0,\n";
        oss << "      \"y\": 0,\n";
        oss << "      \"data\": [\n";

        for (const auto& row : views::reverse(layer))
        {
            oss << "        ";
            for (int tile : row)
            {
                oss << tile << ",";
            }

            oss << "\n";
        }
        std::string temp = oss.str();
        temp.erase(temp.size()-2, 2);
        oss.str(temp);
        oss.seekp(0, std::ios::end);
        oss << "\n      ]\n";
        oss << "    },\n\n";
    }
    std::string tempString = oss.str();
    tempString.erase(tempString.size()-3, 3);
    oss.str(tempString);
    oss.seekp(0, std::ios::end);
    oss << "\n  ]\n";
    oss << "}";

    fileName.append(".json");
    std::string mapString = oss.str();
    downloadMap(fileName.c_str(), mapString.c_str());
}

void Exporter::saveMap(std::string mapName, std::vector<std::string> fileNames, glm::ivec2 tileSize, glm::vec3 RGB,
        std::vector<std::vector<std::vector<int>>> tileMap, std::vector<std::string> layerNames, glm::vec4 gridRGBA)
{
    std::ostringstream oss;
    std::vector<std::string> b64Files;
    std::string saveName = mapName;

    oss << mapName << "\n";                   // Name of the map

    oss << tileMap[0][0].size() << "\n";        // Width of the Map
    oss << tileMap[0].size() << "\n";         // Heihgt of the Map

    // Tilesize
    oss << tileSize.x << "\n";
    oss << tileSize.y << "\n";

    // Clear Color
    oss << RGB[0] << "\n";
    oss << RGB[1] << "\n";
    oss << RGB[2] << "\n";

    // Grid Color
    oss << gridRGBA[0] << "\n";
    oss << gridRGBA[1] << "\n";
    oss << gridRGBA[2] << "\n";
    oss << gridRGBA[3] << "\n";

    oss << fileNames.size() << "\n";            // Amount of Textures
    oss << layerNames.size() << "\n";           // Amount of layers

    // BEGIN - Textures
    for (int i = 0; i < fileNames.size(); i++)
    {
        std::string filePath = fileNames[i];
        std::string encodedFile = m_encoder->encodePNGToB64(filePath);

        b64Files.push_back(encodedFile);
    }

    for (int j = 0; j < fileNames.size(); j++)
    {
        std::string fileName = std::filesystem::path(fileNames[j]).filename().string();

        oss << fileName << "\n";
        oss << b64Files[j] << "\n";
    }
    // END - Textures

    // BEGIN - Tilelayers
    for (int k = 0; k < tileMap.size(); k++)
    {
        std::vector<std::vector<int>> layer = tileMap[k];
        oss << layerNames[k] << "\n";

        for (const auto& row : layer)
        {
            std::streampos rowStart = oss.tellp();

            for (int tile : row)
            {
                oss << tile << " ";
            }
        }

        oss << "\n";
    }
    // END - Tilelayers

    saveName.append(".tile");
    std::string mapString = oss.str();
    downloadMap(saveName.c_str(), mapString.c_str());
}

