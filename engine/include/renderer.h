#pragma once
//GLFW/emscripten/GLES
#include <GLFW/emscripten_glfw3.h>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <GLES3/gl3.h>
#include <emscripten/html5.h>
//Dear ImGui
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
//GLM
#define GLM_FORCE_PURE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>
//STD
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <iomanip>
#include <ranges>
//Project specific
#include "shader.h"
#include "plane.h"
#include "camera.h"
#include "texture.h"
#include "spritebatch.h"
#include "encoder.h"

// BEGIN JavaScript Fuckery

#ifndef RENDERER_H
#define RENDERER_H

#ifdef __cplusplus
extern "C" {
#endif

void downloadFile(const char* filename, const char* text);
void uploadFile();

#ifdef __cplusplus
}
#endif // END extern

#endif // END RENDERER_H

// END JavaScript fuckery

class Renderer {
public:
    Renderer(int width, int height, const char* title);
    ~Renderer();

    void onBeginFrame();
    void onEndFrame();

    //draw commands
    //void drawPlane(); //for simple planes of any size/scale and position
    //for tiles that align to a grid determined by a tileMap
    void drawTile(glm::ivec2& pos, glm::ivec2& size, unsigned int texId, int zLayer, const glm::vec4& color = glm::vec4(1.0f));
    //
    //void drawLine(glm::vec4& color); 
    void drawLine(glm::vec2& start, glm::vec2& end, const glm::vec4& color = glm::vec4(1.0f)); 
    
    // Renderer Getters
    const char* getTitle() { return m_title; }
    int getHeight() { return m_height; }
    int getWidth() { return m_width; }
    float getR() { return r; }
    float getG() { return g; }
    float getB() { return b; }
    auto getTextures() { return m_textures; }
    auto getFiles() { return m_fileNames; }
    float getPickerR() { return pickerR; }
    float getPickerG() { return pickerG; }
    float getPickerB() { return pickerB; }
    int getMaxLayers() { return maxLayers; }
    bool isSafeToImport() { return safeToImport; }
    void loadB64Texture(std::string name, std::string b64);

    std::string getMapString(std::vector<std::vector<std::vector<int>>> tileMap, std::vector<std::string> layerNames, int tileX, int tileY, float mapR, float mapG, float mapB);
    std::string getMapHFILE(std::string filename, std::vector<std::vector<std::vector<int>>> tileMap, std::vector<std::string> layerNames, int tileX, int tileY, float mapR, float mapG, float mapB);
    
    Shader* getShader() { return shader; }
    Camera* getCamera() { return camera; }

    GLFWwindow* getWindow() { return window; }

    void resetTextures();
    void removeTexture(int index);

    // Renderer Setters
    void setTitle(const char* title) { m_title = title; }
    void setHeight(int height) { m_height = height; }
    void setWidth(int width) { m_width = width; }
    void setRGB(float red, float green, float blue) { r = red; g = green; b = blue; }
    void setTextures();
    void setGridColor(const glm::vec4& color);
    
    void setMaxLayers(int z) { sb->setMaxLayers(z);
        maxLayers = z;
    }

private:
    GLFWwindow* window = nullptr;
    Shader* shader;
    Camera* camera;
    SpriteBatch* sb;

    std::vector<std::shared_ptr<Texture>> m_textures;
    std::vector<std::string> m_fileNames;

    GLuint lineVao;
    GLuint lineVbo;

    const char* m_title;
    int m_width;
    int m_height;
    int maxLayers;
    
    bool safeToImport = false;

    float r, g, b;
    float pickerR, pickerG, pickerB;
};