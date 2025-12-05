#include "application.h"
#define GLM_FORCE_PURE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include <vector>
#include "input.h"
#include "exporter.h"
#include "importer.h"

std::function<void()> importFinishedCallback = nullptr;

void setImportFinishedCallback(std::function<void()> callback)
{
    importFinishedCallback = std::move(callback);
}

extern "C" {
EMSCRIPTEN_KEEPALIVE
void onImportFinished()
{
    if (importFinishedCallback)
    {
        importFinishedCallback();
    }
}
}

EM_JS(int, canvas_get_width, (), {
  return Module.canvas.width;
});

EM_JS(int, canvas_get_height, (), {
  return Module.canvas.height;
});

struct tileLayerBar {
  char text[128];
  int zIndex;
};

int width, height;
GLuint selectedTexture = -1;
int selectedTexIndex = -1;
int selectedLayer = 0;
static bool initialized = false;
static bool init = false;
static bool openClearMap = false;
static bool openLoadProject = false;
static bool shouldResetColor = false;
static bool shouldImport = false;

//structi scroll_callback pointterille
struct CameraCallbackData {
  glm::vec2 camPosDelta;
  glm::vec2 pos;
  float CameraZoom = 1.0f;
  float minZoom = 0.1f;
  float maxZoom = 4.9f;

  glm::vec2 screenToWorld(const glm::vec2& screen, const glm::ivec2& windowSize) const {
    glm::vec2 ndc = (screen / glm::vec2(windowSize)) * 2.0f - glm::vec2(1.0f, 1.0f);
    ndc.y = -ndc.y;
    glm::vec2 world = pos + ndc * (glm::vec2(windowSize) * 0.5f) * CameraZoom;
    return world;
  }
};

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  CameraCallbackData* cbData = static_cast<CameraCallbackData*>(glfwGetWindowUserPointer(window));
  if(!cbData) {
    return;
  }

  cbData->camPosDelta = {0.0f, 0.0f};
  glm::ivec2 wSize;
  glfwGetWindowSize(window, &wSize.x, &wSize.y);
  double mx, my;
  glfwGetCursorPos(window, &mx, &my);

  //mouse world pos before zoom

  //zoom
  const float zoomFactor = 0.1f;
  float newCameraZoom = cbData->CameraZoom;
  newCameraZoom += (yoffset > 0.0) ? -zoomFactor : (zoomFactor);

  //limit camera zoom
    if(newCameraZoom < cbData->minZoom) {
      newCameraZoom = cbData->minZoom;
    }
    if(newCameraZoom > cbData->maxZoom) {
      newCameraZoom = cbData->maxZoom;
    }

  cbData->CameraZoom = newCameraZoom;
  
  glm::vec2 mousePos = glm::vec2((float)mx, (float)my);
  mousePos = cbData->screenToWorld(mousePos, wSize);

  //shift camera
  cbData->camPosDelta = (yoffset > 0.0) ? (glm::lerp(mousePos, cbData->pos, 0.9f) - cbData->pos) : glm::vec2(0.f, 0.f);
}

class TestApp : public Application
{
public:
  TestApp() : Application(width, height, "Tile - Tile Is a Level Editor")
  {
      renderer = TestApp::getRenderer();
      cbData.pos = renderer->getCamera()->getPosition();
      glfwSetWindowUserPointer(renderer->getWindow(), &cbData);
      glfwSetScrollCallback(renderer->getWindow(), scroll_callback);
      myInputs = std::make_unique<InputSystem>(renderer->getWindow());
      myExporter = std::make_unique<Exporter>();
      myImporter = std::make_unique<Importer>();
      glm::vec4 gridColor = {0.8f, .8f, .8f, 1.0f};
      logoID = loadLogo();
  }

  // BEGIN - User Interface
  void newBar(std::string text, int index)
  {
      tileLayerBar tempBar;
      strncpy(tempBar.text, text.c_str(), sizeof(tempBar.text)-1);
      tempBar.text[sizeof(tempBar.text)-1] = '\0';
      tempBar.zIndex = index;
      bars.push_back(tempBar);
  }

  GLuint loadLogo()
  {
      int logoWidth, logoHeight, nrChannels;
      unsigned char* texData = stbi_load("assets/logo.png", &logoWidth, &logoHeight, &nrChannels, STBI_rgb_alpha);

      std::shared_ptr<Texture> tempTexture = std::make_shared<Texture>(logoWidth, logoHeight, nrChannels, texData);
      stbi_image_free(texData);

      logoTexture = tempTexture;

      return tempTexture->getTextureID();
  }

  void createMap()
  {
      static int tileX = 64;
      static int tileY = 64;
      static int width = 15;
      static int height = 10;
      static int layers = 1;

      ImGui::OpenPopup("Create Map");

      if (ImGui::BeginPopupModal("Create Map", NULL, ImGuiWindowFlags_AlwaysAutoResize))
      {
          // User can edit the Project Name
          static char inputBuffer[128] = "Unnamed Tilemap";
          ImGui::AlignTextToFramePadding();
          ImGui::Text("Project Name:");
          ImGui::SameLine();
          ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
          ImGui::InputText("##NameInput", inputBuffer, IM_ARRAYSIZE(inputBuffer));
          ImGui::PopItemWidth();
          ImGui::Separator();

          // User can set the Map Dimensions
          ImGui::AlignTextToFramePadding();
          ImGui::Text("Map width:");
          ImGui::SameLine();
          ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
          ImGui::InputInt("##mhInput", &width);
          ImGui::PopItemWidth();

          ImGui::AlignTextToFramePadding();
          ImGui::Text("Map height:");
          ImGui::SameLine();
          ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
          ImGui::InputInt("##mwInput", &height);
          ImGui::PopItemWidth();
          ImGui::Separator();

          // User can set the initial amount of tile layers
          ImGui::AlignTextToFramePadding();
          ImGui::Text("Tile layers:");
          ImGui::SameLine();
          ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
          if (ImGui::InputInt("##lInput", &layers)) { if (layers < 1) { layers = 1; } }
          ImGui::PopItemWidth();
          ImGui::Separator();

          // User can set the dimensons of a tile
          ImGui::AlignTextToFramePadding();
          ImGui::Text("Tile width:");
          ImGui::SameLine();
          ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
          ImGui::InputInt("##twInput", &tileX);
          ImGui::PopItemWidth();

          ImGui::AlignTextToFramePadding();
          ImGui::Text("Tile height:");
          ImGui::SameLine();
          ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
          ImGui::InputInt("##thInput", &tileY);
          ImGui::PopItemWidth();
          ImGui::Separator();

          // Tells the user how large the map will be
          ImGui::Text("%d x %d Pixels", width*tileX, height*tileY);

          ImGui::Spacing();

          // If the user is ready to start making a tilemap
          if (ImGui::Button("OK", ImVec2(120, 0)))
          {
              initialized = true;
              renderer->resetTextures();
              myTextures.clear();

              r = 0.0f;
              g = 0.0f;
              b = 0.0f;
              shouldResetColor = true;

              selectedTexIndex = -1;
              selectedTexture = -1;
              
              myTileMap.m_mapSize = glm::ivec2(width, height);
              myTileMap.m_tileSize = glm::ivec2(tileX, tileY);
              
              bars.clear();
              myTileMap.m_mapLayers.clear();
              myTileMap.clearMap();
              
              myTileMap.m_mapName = inputBuffer;

              for (int n = 0; n < layers; n++)
              {
                  tileMap::tileLayer tempLayer = tileMap::tileLayer(
                  myTileMap.m_mapSize.y,
                  std::vector<int>(myTileMap.m_mapSize.x, -1)
                  );

                  myTileMap.m_mapLayers.push_back(tempLayer);
                  myTileMap.m_layerNames.push_back("Layer_" + std::to_string(myTileMap.m_mapLayers.size()-1));
                  newBar("Layer_" + std::to_string(myTileMap.m_mapLayers.size()-1), myTileMap.m_mapLayers.size()-1);
              }

              float gridWidth = myTileMap.m_mapSize.x * myTileMap.m_tileSize.x;
              float gridHeight = myTileMap.m_mapSize.y * myTileMap.m_tileSize.y;

              glm::vec2 gridCenter;
              gridCenter.x = gridWidth * 0.5f;
              gridCenter.y = gridHeight * 0.5f;

              glm::vec2 camPos;
              camPos.x = gridCenter.x + (337/2);
              camPos.y = gridCenter.y;

              renderer->getCamera()->setPosition(camPos);
              isRunning = true;
              ImGui::CloseCurrentPopup();
          }

          ImGui::SameLine();
          
          // If the user wants to go back to the previous map
          if (ImGui::Button("Cancel", ImVec2(120, 0)) && initialized)
          {
            isRunning = true;
            ImGui::CloseCurrentPopup();
          }

        ImGui::EndPopup();
      }
  }

  void renderUI()
  {
      ImGuiIO& io = ImGui::GetIO();
      ImVec2 displaySize = io.DisplaySize;

      float sbHeight = 25.f; 
      ImGui::SetNextWindowPos(ImVec2(0, displaySize.y - sbHeight), ImGuiCond_Always);
      ImGui::SetNextWindowSize(ImVec2(displaySize.x, sbHeight), ImGuiCond_Always);

      ImGuiWindowFlags statusBarFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;

      if (ImGui::Begin("StatusBar", nullptr, statusBarFlags))
      {
          ImGuiViewport* viewport = ImGui::GetMainViewport();
          //Camera position in tilemap

          ImVec2 mousePos = io.MousePos;
          glm::ivec2 wSize;
          glfwGetWindowSize(renderer->getWindow(), &wSize.x, &wSize.y);
          double mx, my;
          glfwGetCursorPos(renderer->getWindow(), &mx, &my);
          glm::vec2 mousePos2 = glm::vec2((float)mx, (float)my);
          mousePos2 = cbData.screenToWorld(mousePos2, wSize);

          glm::ivec2 gridPos = getMousePosInGrid(mousePos, myTileMap);

          if (gridPos.y <= myTileMap.m_mapSize.y-1 && gridPos.x <= myTileMap.m_mapSize.x-1 && gridPos.x >= 0)
          {
              if (myTileMap.getTileId(selectedLayer, gridPos.x, gridPos.y) == -1)
              {
                  ImGui::Text("(%d, %d) - [Empty] - Scale: (%.1fpx / 1px)", gridPos.x, gridPos.y, currentCameraZoom);
              }

              else
              {
                  int id = myTileMap.getTileId(selectedLayer, gridPos.x, gridPos.y);
                  ImGui::Text("(%d, %d) - [%d] - Scale: (%.1fpx / 1px)", gridPos.x, gridPos.y, id, currentCameraZoom);
              }
          }

          else
          {
              ImGui::Text("(%d, %d) - [Empty] - Scale: (%.1fpx / 1px)", gridPos.x, gridPos.y, currentCameraZoom);
          }
        
      }

      ImGui::End();

      // Tehdään Action Bar MainMenuBarin alapuolelle
      const float abHeight = 75.f;
      ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
      ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, abHeight), ImGuiCond_Always);

      ImGuiWindowFlags toolbarFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                                      ImGuiWindowFlags_NoSavedSettings;

      if (ImGui::Begin("Actionbar", nullptr, toolbarFlags))
      {
          ImVec2 size = ImVec2(104, 64);
          ImTextureID logoTex = logoID;
          ImGuiStyle& style = ImGui::GetStyle();
          ImVec2 windowSize = ImGui::GetWindowSize();
          
          float centerY = (windowSize.y - 64) * 0.5f;
          ImGui::SetCursorPosY(centerY);
          ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

          if (ImGui::ImageButton("Logo", logoTex, size, ImVec2(0,0), ImVec2(1,1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1))) 
          { 
            ImGui::OpenPopup("About"); 
          }

          ImGui::PopStyleVar();

          ImVec2 aboutPos = ImGui::GetItemRectMax();
          ImVec2 aboutSize = ImGui::GetItemRectSize();
          ImVec2 aboutPopup = { aboutPos.x - aboutSize.x, aboutPos.y + aboutSize.y - 59.0f };

          ImGui::SameLine();
          
          float centerY2 = (windowSize.y - 35) * 0.5f;
          ImGui::SetCursorPosY(centerY2);
          ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);

          if (ImGui::Button(myTileMap.m_mapName.empty() ? "##EmptyID" : myTileMap.m_mapName.c_str(), ImVec2(0, 35))) 
          { 
            ImGui::OpenPopup("Project"); 
          }

          ImVec2 projectPos = ImGui::GetItemRectMax();
          ImVec2 projectSize = ImGui::GetItemRectSize();
          ImVec2 projectPopup = { projectPos.x - projectSize.x, projectPos.y + projectSize.y - 16.0f };

          float startX = ImGui::GetContentRegionAvail().x - 100 - 308;
          ImGui::SameLine(startX);

          ImGui::SetCursorPosY(centerY2);

          if (ImGui::Button("Export", ImVec2(100, 35))) 
          { 
            ImGui::OpenPopup("Export");
          }

          ImVec2 exportPos = ImGui::GetItemRectMin();
          ImVec2 exportSize = ImGui::GetItemRectSize();
          ImVec2 exportPopup = { exportPos.x + exportSize.x, exportPos.y + exportSize.y + 19.0f};

          float nextX = startX - 100 - 10;
          ImGui::SameLine(nextX);

          ImGui::SetCursorPosY(centerY2);
          
          if (ImGui::Button("Controls", ImVec2(100, 35))) 
          { 
              ImGui::OpenPopup("Controls");
          }

          ImVec2 controlsPos = ImGui::GetItemRectMin();
          ImVec2 controlsSize = ImGui::GetItemRectSize();
          ImVec2 controlsPopup = { controlsPos.x + controlsSize.x, controlsPos.y + controlsSize.y + 19.0f};

          // BEGIN Controls
          ImGui::SetNextWindowPos(controlsPopup, 0, ImVec2(1.0f, 0.0f));
          if (ImGui::BeginPopup("Controls", ImGuiWindowFlags_AlwaysAutoResize))
          {
              ImGui::AlignTextToFramePadding();
              ImGui::Text("Controls");
              ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 25);

              if (ImGui::Button("X", ImVec2(25, 0))) 
              { 
                  ImGui::CloseCurrentPopup();
              }

              ImGui::Separator();

              ImGui::Text("Left MB - Select / Interact / Draw tiles");
              ImGui::Text("Right MB - Deselect / Erase tles");
              ImGui::Text("Middle MB - Scroll to zoom / Click to pan");

              ImGui::EndPopup();
          }
          // END Controls

        // BEGIN Export
        ImGui::SetNextWindowPos(exportPopup, 0, ImVec2(1.0f, 0.0f));

        if (ImGui::BeginPopup("Export", ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Export");
            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 25);

            if (ImGui::Button("X", ImVec2(25, 0))) 
            { 
                ImGui::CloseCurrentPopup();
            }

            ImGui::Separator();

            if (ImGui::Button("Text File", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
            {
                glm::ivec2 size = glm::ivec2(myTileMap.m_tileSize.x, myTileMap.m_tileSize.y);
                glm::vec3 rgb = glm::vec3(r, g, b);

                myExporter->exportToTxt(myTileMap.m_mapName, myTileMap.m_mapLayers, myTileMap.m_layerNames,
                renderer->getFiles(), size, rgb);
            }

            if (ImGui::Button("Header File", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
            {
                glm::ivec2 size = glm::ivec2(myTileMap.m_tileSize.x, myTileMap.m_tileSize.y);
                glm::vec3 rgb = glm::vec3(r, g, b);

                myExporter->exportToHFile(myTileMap.m_mapName, myTileMap.m_mapLayers, myTileMap.m_layerNames,
                renderer->getFiles(), size, rgb);
            }

            if (ImGui::Button("JSON", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
            {
                glm::ivec2 size = glm::ivec2(myTileMap.m_tileSize.x, myTileMap.m_tileSize.y);
                myExporter->exportToJSON(myTileMap.m_mapName, myTileMap.m_mapLayers, myTileMap.m_layerNames, size);
            }

            ImGui::EndPopup();

        }
        // END Export

        // BEGIN Project
        ImGui::SetNextWindowPos(projectPopup, 0, ImVec2(0.0f, 0.0f));

        if (ImGui::BeginPopup("Project", ImGuiWindowFlags_AlwaysAutoResize))
        {
            static char nameBuffer[64];
            static float newColor[3] = {r, g, b};
            static float newGrid[4] = {gridColor[0], gridColor[1], gridColor[2], gridColor[3]}; 

            if (shouldResetColor)
            {
                newColor[0] = 0.0f;
                newColor[1] = 0.0f;
                newColor[2] = 0.0f;

                newGrid[0] = 1.0f;
                newGrid[1] = 1.0f;
                newGrid[2] = 1.0f;
                newGrid[3] = 1.0f;

                r = 0.0f;
                g = 0.0f;
                b = 0.0f;

                gridColor = {1.0f, 1.0f, 1.0f, 1.0f};
                shouldResetColor = false;
            }

            if (!init)
            {
                std::strncpy(nameBuffer, myTileMap.m_mapName.c_str(), sizeof(nameBuffer));
                nameBuffer[sizeof(nameBuffer)-1] = '\0';

                newColor[0] = r;
                newColor[1] = g;
                newColor[2] = b;

                init = true;
            }

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Project");
            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 25);

            if (ImGui::Button("X", ImVec2(25, 0))) 
            { 
                ImGui::CloseCurrentPopup();
                init = false;
            }

            ImGui::Separator();

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Project Name:");
            ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::InputText("##NameInput2", nameBuffer, IM_ARRAYSIZE(nameBuffer));
            ImGui::PopItemWidth();

            ImGui::Separator();

            ImGui::AlignTextToFramePadding();
            ImGui::Text("BG Color:");
            ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

            if (ImGui::ColorEdit3("##ColorInput", newColor))
            {
                r = newColor[0];
                g = newColor[1];
                b = newColor[2];
            }
            
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Grid Color:");
            ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

            if (ImGui::ColorEdit4("##GridInput", newGrid))
            {
                gridColor[0] = newGrid[0];
                gridColor[1] = newGrid[1];
                gridColor[2] = newGrid[2];
                gridColor[3] = newGrid[3];
            }

            ImGui::PopItemWidth();

            ImGui::Separator();

            if (ImGui::Button("Clear Map", ImVec2(120, 0))) 
            {
                openClearMap = true;
                ImGui::CloseCurrentPopup(); 
            }

            ImGui::SameLine();

            if (ImGui::Button("New Project", ImVec2(120, 0))) { isRunning = false; }

            ImGui::Separator();
            
            if (ImGui::Button("Save Project", ImVec2(120, 0)))
            {
                glm::ivec2 size = glm::ivec2(myTileMap.m_tileSize.x, myTileMap.m_tileSize.y);
                glm::vec3 rgb = glm::vec3(r, g, b);
                myExporter->saveMap(myTileMap.m_mapName, renderer->getFiles(), size, rgb, 
                myTileMap.m_mapLayers, myTileMap.m_layerNames, gridColor);
            }

            ImGui::SameLine();

            if (ImGui::Button("Load Project", ImVec2(120, 0)))
            {
                openLoadProject = true;
                ImGui::CloseCurrentPopup();
            }
            
            myTileMap.m_mapName = nameBuffer;
            ImGui::EndPopup();
        }
        // END Project
        
        // BEGIN Clear Map
        if (openClearMap)
        {
            ImGui::OpenPopup("Clear Map");
            openClearMap = false;
        }
        
        if (ImGui::BeginPopupModal("Clear Map", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
        {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Are you sure you want to clear your map?");
            ImGui::Text("Your current work will not be saved!");
            ImGui::Separator();

          if(ImGui::Button("Yes, I'm sure!", ImVec2(160, 0))) 
          { 
              r = 0.0f;
              g = 0.0f;
              b = 0.0f;

              shouldResetColor = true;
              bars.clear();
              myTileMap.m_mapLayers.clear();
              myTileMap.clearMap();

              ImGui::CloseCurrentPopup();
          }

            ImGui::SameLine();

              if(ImGui::Button("No, take me back!", ImVec2(160, 0))) 
              {
                  ImGui::CloseCurrentPopup();
              }

              ImGui::EndPopup();
            }
            // END Clear Map

        // BEGIN Load Project
        if (openLoadProject)
        {
            ImGui::OpenPopup("Load Project");
            openLoadProject = false;
        }

        if (ImGui::BeginPopupModal("Load Project", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
        {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Are you sure you want to proceed with loading a previous project?");
            ImGui::Text("Your current project will NOT be saved.");
            ImGui::Separator();

            if(ImGui::Button("Save & Proceed", ImVec2(160, 0)))
            {
                glm::ivec2 size = glm::ivec2(myTileMap.m_tileSize.x, myTileMap.m_tileSize.y);
                glm::vec3 rgb = glm::vec3(r, g, b);
                myExporter->saveMap(myTileMap.m_mapName, renderer->getFiles(), size, rgb, 
                myTileMap.m_mapLayers, myTileMap.m_layerNames, gridColor);

                shouldImport = true;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if(ImGui::Button("Proceed", ImVec2(160, 0)))
            {
                shouldImport = true;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if(ImGui::Button("Cancel", ImVec2(160, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        // END Load Project

        // BEGIN About
        ImGui::SetNextWindowPos(aboutPopup, 0, ImVec2(0.0f, 0.0f));
        if (ImGui::BeginPopup("About", ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Tile - Tile Is a Level Editor");
            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 25);

            if (ImGui::Button("X", ImVec2(25, 0))) { ImGui::CloseCurrentPopup(); }

            ImGui::Separator();

            ImGui::Text("Peliteknologian Projekti 3 - Tiimi 1");
            ImGui::Separator();
            ImGui::Text("Developed by:");
            ImGui::Text("Eetu Kuru TTV23SP");
            ImGui::Text("Julius Muurinen TTV23SP");
            ImGui::Separator();
            ImGui::Text("Special thanks to:");
            ImGui::Text("Daniel Rinne");
            ImGui::Text("Mikko Romppainen");
            ImGui::Text("Niko Kinnunen");
            ImGui::Separator();
            ImGui::Text("\"THE BEERWARE LICENSE\" (Revision 42):");
            ImGui::Text("<tiimi1@kamk> wrote this code. As long you retain this");
            ImGui::Text("notice, you can do whatever you want with this stuff.");
            ImGui::Text("If we meet someday, and you think this stuff is worth it,");
            ImGui::Text("you can buy us beer(s) in return.");
            ImGui::Separator();
            ImGui::Text("Copyright © 2025 Eetu Kuru & Julius Muurinen");
            ImGui::EndPopup();
        }
        // END About
      }
      
      ImGui::End();

      // Kutsutaan Tilesettien content UIta
      tilesetUI(0, sbHeight-1, abHeight-1);
  }

  void tilesetUI(float abHeight, float ibHeight, float mbHeight)
  {
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        // Aloitetaan piirtäminen menu- ja action barien alapuolelta
        float contentY = viewport->Pos.y + mbHeight + abHeight;

        // Ikkunan koko on koko ruutu miinus palkkien korkeudet
        float wHeight = viewport->Size.y - (abHeight + ibHeight + mbHeight);

        float wWidth = 337.f;
        float contentX = viewport->Pos.x + viewport->Size.x - wWidth;

        ImVec2 wPos = ImVec2(contentX, contentY);
        ImVec2 wSize = ImVec2(wWidth, wHeight);

        ImGui::SetNextWindowPos(wPos);
        ImGui::SetNextWindowSize(wSize);

        ImGuiWindowFlags wFlags = ImGuiWindowFlags_NoResize
                                  | ImGuiWindowFlags_NoMove
                                  | ImGuiWindowFlags_NoCollapse
                                  | ImGuiWindowFlags_NoScrollbar
                                  | ImGuiWindowFlags_NoTitleBar;

        ImGui::Begin("Tiles", nullptr, wFlags);

        float splitRatio = 0.50f;
        float footerHeight = ImGui::GetFrameHeightWithSpacing();
        ImVec2 avail = ImGui::GetContentRegionAvail();
        float c1Height = std::floor(avail.y * splitRatio);
        float c2Height = avail.y - c1Height - ImGui::GetStyle().ItemSpacing.y;

        ImGui::BeginChild("Properties", ImVec2(0, c1Height), true);
        float propHeight = ImGui::GetFrameHeightWithSpacing();
        ImVec2 propAvail = ImGui::GetContentRegionAvail();

        ImGui::BeginChild("ScrollableBars", ImVec2(0, propAvail.y - propHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
          for (int i = 0; i < myTileMap.m_mapLayers.size(); i++)
          {
              strncpy(bars[i].text, myTileMap.m_layerNames[i].c_str(), sizeof(bars[i].text)-1);
              bars[i].text[sizeof(bars[i].text)-1] = '\0';
              bars[i].zIndex = i;
              // Make the bar fill the width of the child
              float full_width = ImGui::GetContentRegionAvail().x;

              ImGui::PushID(10+i);

              static bool check;

              if (bars[i].zIndex == selectedLayer) { check = true; }
              else { check = false; }

              // Compute button sizes
              float button_width = 30.f;
              float spacing = ImGui::GetStyle().ItemSpacing.x;
              float total_buttons_width = (20 + 20 + 20 + 50) + (spacing * 3);
              float input_width = full_width - total_buttons_width - spacing;

              ImGui::BeginDisabled();
              ImGui::Checkbox("##Active", &check);
              ImGui::EndDisabled();

              ImGui::SameLine();

              // Text input
              ImGui::SetNextItemWidth(input_width);
              if (ImGui::InputText("##input", bars[i].text, sizeof(bars[i].text)))
              {
                  myTileMap.m_layerNames[i] = std::string(bars[i].text);
              }

              ImGui::SameLine();

              if (ImGui::Button("Select", ImVec2(50, 0))) { selectedLayer = i; }

              ImGui::SameLine();

              // Buttons
              if (ImGui::Button("^", ImVec2(20, 0))) {
                if (i > 0)
                {
                  std::swap(bars[i], bars[i-1]);
                  std::swap(myTileMap.m_mapLayers[i], myTileMap.m_mapLayers[i-1]);
                  std::swap(myTileMap.m_layerNames[i], myTileMap.m_layerNames[i-1]);  
                  
                  if (i == selectedLayer) { selectedLayer--; }
                }
              }
              ImGui::SameLine();
              if (ImGui::Button("v", ImVec2(20, 0))) { 
                if (i < bars.size()-1)
                {
                  std::swap(bars[i], bars[i+1]);
                  std::swap(myTileMap.m_mapLayers[i], myTileMap.m_mapLayers[i+1]);
                  std::swap(myTileMap.m_layerNames[i], myTileMap.m_layerNames[i+1]);  
                  if (i == selectedLayer) { selectedLayer++; }
                }
              }

              ImGui::PopID();
          }
        
        ImGui::EndChild();

        float childWidth = ImGui::GetContentRegionAvail().x;
        float btnWidth = (childWidth - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

        if (ImGui::Button("New Layer", ImVec2(btnWidth, 0))) { 

            tileMap::tileLayer tempLayer = tileMap::tileLayer(
            myTileMap.m_mapSize.y,
            std::vector<int>(myTileMap.m_mapSize.x, -1)
            );

            myTileMap.m_mapLayers.push_back(tempLayer);
            myTileMap.m_layerNames.push_back("Layer_" + std::to_string(myTileMap.m_mapLayers.size()-1));
            newBar("Layer_" + std::to_string(myTileMap.m_mapLayers.size()-1), myTileMap.m_mapLayers.size()-1);
        }

        ImGui::SameLine();

        if (ImGui::Button("Remove Layer", ImVec2(btnWidth, 0))) 
        { 
          if (bars.size() > 1 && selectedLayer != -1)
          {
            bars.erase(bars.begin() + selectedLayer);
            myTileMap.m_mapLayers.erase(myTileMap.m_mapLayers.begin() + selectedLayer);
            selectedLayer = -1;

            for (int i = 0; i < bars.size(); i++)
            {
              if (bars[i].zIndex == 0) { continue; }

              else
              {
                bars[i].zIndex = bars[i].zIndex-1;
              }

            }
          }
        }

        ImGui::EndChild();

        ImGui::BeginChild("Tileset", ImVec2(0, c2Height), true);
        float tilesetHeight = ImGui::GetFrameHeightWithSpacing();
        ImVec2 tilesetAvail = ImGui::GetContentRegionAvail();

        ImGui::BeginChild("ScrollableTextures", ImVec2(0, tilesetAvail.y - tilesetHeight), true, ImGuiWindowFlags_HorizontalScrollbar);

        ImGuiStyle& style = ImGui::GetStyle();
        ImVec2 prevSpacing = style.ItemSpacing;
        style.ItemSpacing = ImVec2(0, 0);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        for (int i = 0; i < myTextures.size(); i++)
        {
          if (i % 9 != 0) { ImGui::SameLine(); }
          ImGui::PushID(std::to_string(i).c_str());
          ImTextureID tex = myTextures[i]->getTextureID();
          ImVec2 size = ImVec2(32, 32);

          ImVec4 tint = (selectedTexIndex == i)
                        ? ImVec4(0.7f, 0.7f, 0.7f, 1.0f)
                        : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

          if (ImGui::ImageButton(std::to_string(i).c_str(), tex, size, ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), tint));

            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                selectedTexture = tex;
                selectedTexIndex = i;
            }

            if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
            {
                selectedTexture = -1;
                selectedTexIndex = -1;
            }

          ImGui::PopID();
        }
        ImGui::PopStyleVar();
        ImGui::EndChild();

        style.ItemSpacing = prevSpacing;
        ImGui::Spacing();

        float fullWidth = ImGui::GetContentRegionAvail().x;
        float buttonWidth = (fullWidth - ImGui::GetStyle().ItemSpacing.x) * 0.5f;

        if (ImGui::Button("Import Texture", ImVec2(buttonWidth, 0))) {
            ImGui::OpenPopup("Import Texture");
        }

        ImGui::SameLine();
        if (ImGui::Button("Remove Texture", ImVec2(buttonWidth, 0))) 
        { 
          if (myTextures.size() > 0 && selectedTexIndex != -1)
          {
            ImGui::OpenPopup("Remove Texture");
          }
        }

        if (ImGui::BeginPopupModal("Import Texture", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) 
        {
          ImGui::AlignTextToFramePadding();
          ImGui::Text("Which kind of texture do you want to import?");
          ImGui::Separator();
          ImGui::Text("Maximum File size: 2MB");
          ImGui::Separator();

          if(ImGui::Button("Single Texture", ImVec2(160, 0)))
            { 
              uploadFile();
              ImGui::CloseCurrentPopup();
            }

          ImGui::SameLine();

          if(ImGui::Button("Cancel", ImVec2(160, 0)))
            { 
              ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal("Remove Texture", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
        {
            std::string warning = "Are you sure you want to remove Texture ID: " + std::to_string(selectedTexIndex) + "?";
            ImGui::AlignTextToFramePadding();
            ImGui::Text("%s", warning.c_str());
            ImGui::Separator();
            ImGui::Text("Removing a texture also removes all tiles");
            ImGui::Text("that are associated with that it!");
            ImGui::Separator();
            if (ImGui::Button("Yes, I'm sure!", ImVec2(160, 0)))
            { 
                myTextures.erase(myTextures.begin() + selectedTexIndex);
                renderer->removeTexture(selectedTexIndex);

                myTileMap.removeTextureFromMap(selectedTexIndex);

                selectedTexIndex = -1;
                selectedTexture = -1;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("No, take me back!", ImVec2(160, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
        ImGui::EndChild();

        ImGui::End();
  }
  // END - User Interface

    glm::ivec2 getMousePosInGrid(ImVec2 mousePos, layeredTileMap map) {
      int windowWidth = renderer->getWidth();
      int windowHeight = renderer->getHeight();
      ImVec2 mousePosOnScreen = {mousePos.x, windowHeight - mousePos.y}; //mouse pixel position on screen relative to the bottom-left corner
      glm::vec2 camPos = renderer->getCamera()->getPosition(); //Camera position in world (starts from {0,0})
      glm::vec2 mousePosInWorld = {
        (mousePosOnScreen.x * currentCameraZoom) + camPos.x - (windowWidth * currentCameraZoom / 2.0f), 
        (mousePosOnScreen.y * currentCameraZoom) + camPos.y - (windowHeight * currentCameraZoom / 2.0f)
      }; //mouse position in world
    
      float SizeX = map.m_tileSize.x;
      float SizeY = map.m_tileSize.y;

      int tileX = static_cast<int>(std::floor(mousePosInWorld.x/ SizeX));
      int tileY = static_cast<int>(std::floor(mousePosInWorld.y/ SizeY));

      return glm::ivec2(tileX, tileY);
    }

    void render() override
    {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      
      renderer->setRGB(r, g, b);
      renderer->setGridColor(gridColor);

      if (!isRunning)
      {
          createMap();
      }

      if (isRunning)
      {
        renderer->setTextures();
        myTextures = renderer->getTextures();

        renderUI();
        myTileMap.renderMap(renderer);
        myTileMap.drawGrid(renderer, gridColor);
      }

      glDisable(GL_BLEND);
    }

    void update(float deltaTime) override {
        glm::vec2 prevCamPos = renderer->getCamera()->getPosition();
        cbData.pos = prevCamPos;
        
        glm::vec2 cameraDelta(0,0);
        ImGuiIO& io = ImGui::GetIO();

        //Camera scroll update happens in scroll_callback, but we update cameraZoom here:
        currentCameraZoom = cbData.CameraZoom;
        renderer->getCamera()->setScale(currentCameraZoom);
        

        cameraDelta += (cbData.camPosDelta);
        cbData.camPosDelta = {0,0};

        //Add tile if valid texture
        if (myInputs->isButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && selectedTexture != -1 && !io.WantCaptureMouse)
        {
          ImGuiViewport* viewport = ImGui::GetMainViewport();
          //Cursor position in viewport
          ImVec2 mousePos = ImGui::GetIO().MousePos;
          glm::ivec2 gridPos = getMousePosInGrid(mousePos, myTileMap);

          if ( gridPos.y <= myTileMap.m_mapSize.y - 1 && gridPos.x <= myTileMap.m_mapSize.x -1 && gridPos.x >= 0)
          {
            myTileMap.setTileId(selectedLayer, gridPos.x, gridPos.y, selectedTexIndex);
          }
        }

        //Remove tile
        if (myInputs->isButtonPressed(GLFW_MOUSE_BUTTON_RIGHT) && !io.WantCaptureMouse) {
          ImGuiViewport* viewport = ImGui::GetMainViewport(); 
          ImVec2 mousePos = ImGui::GetIO().MousePos;
        
          glm::ivec2 gridPos = getMousePosInGrid(mousePos, myTileMap);

          if ( gridPos.y <= myTileMap.m_mapSize.y - 1 && gridPos.x <= myTileMap.m_mapSize.x -1 && gridPos.x >= 0)
          {
            myTileMap.setTileId(selectedLayer, gridPos.x, gridPos.y, -1);
          }
        }

        //Middle mouse button delta
        if(myInputs->isButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE) && !io.WantCaptureMouse) {

          ImVec2 mouseDelta = io.MouseDelta;
          mouseDelta.x *= currentCameraZoom;
          mouseDelta.y *= currentCameraZoom;
          cameraDelta -= glm::vec2(mouseDelta.x, -mouseDelta.y);
        }
        renderer->getCamera()->move(cameraDelta);

        if(shouldImport)
        {
            myImporter->startImport();

            setImportFinishedCallback([this]() {
            if (this->myImporter->saveData())
                this->pendingMapLoad = true;
            });

            shouldImport = false;
        }

        if (pendingMapLoad && renderer->isSafeToImport())
        {
            loadSavedMap();
            pendingMapLoad = false;
            init = false;
        }
    }

    void loadSavedMap()
    {
        std::vector<std::string> tempTexNames;
        std::vector<std::string> tempTextures;

        if (!myImporter) return;

        myTextures.clear();
        renderer->resetTextures();

        myTileMap.m_mapName = myImporter->getMapName();
        myTileMap.m_tileSize.x = myImporter->getTileX();
        myTileMap.m_tileSize.y = myImporter->getTileY();
        myTileMap.m_mapSize.x = myImporter->getWidth();
        myTileMap.m_mapSize.y = myImporter->getHeight();

        r = myImporter->getR();
        g = myImporter->getG();
        b = myImporter->getB();

        gridColor.r = myImporter->getGridR();
        gridColor.g = myImporter->getGridG();
        gridColor.b = myImporter->getGridB();
        gridColor.a = myImporter->getGridA();

        tempTexNames = myImporter->getTexNames();
        tempTextures = myImporter->getB64Strings();

        size_t totalTextures = tempTexNames.size();

        for (size_t i = 0; i < totalTextures; i++)
        {
            renderer->loadB64Texture(tempTexNames[i], tempTextures[i]);
        }

        myTextures = renderer->getTextures();
        
        myTileMap.m_layerNames = myImporter->getLayerNames();
        myTileMap.m_mapLayers = myImporter->getLayers();
        bars.resize(myTileMap.m_layerNames.size());
    }

private:
    float r = 0.f;
    float g = 0.f;
    float b = 0.f;
    double currentCameraZoom = 1.0;
    CameraCallbackData cbData;
    Renderer* renderer;
    Plane* myPlane;
    std::vector<Plane*> myPlanes;
    std::vector<std::shared_ptr<Texture>> myTextures;
    std::shared_ptr<Texture> logoTexture;
    GLuint logoID;
    std::unique_ptr<InputSystem> myInputs;
    layeredTileMap myTileMap;
    std::unique_ptr<Exporter> myExporter;
    std::unique_ptr<Importer> myImporter;
    std::vector<tileLayerBar> bars;
    glm::vec4 gridColor = {1.0f, 1.0f, 1.0f, 1.0f};
    bool isRunning = false;
    bool shouldClear = false;
    bool pendingMapLoad = false;
};

TestApp* app = 0;

int main()
{
  width = canvas_get_width();
  height = canvas_get_height();

  app = new TestApp;
  app->run();

  return 0;
}