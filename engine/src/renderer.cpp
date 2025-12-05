#include "renderer.h"

// KAMK Värit
// Sininen: (0.004f * 2, 0.004f * 116, 0.004f * 131)
// Tumman harmaa: (0.004f * 236, 0.004f * 233, 0.004f * 228)
// Vaalean harmaa: (0.004f * 242, 0.004f * 240, 0.004f* 237)
// Musta: (0.004f * 51, 0.004f * 51, 0.004f * 51)

// This breaks the geneva convention, too bad!
std::vector<std::shared_ptr<Texture>> tempTextures;
std::vector<std::string> tempNames;


EM_JS(void, downloadFile, (const char* filename, const char* text), {
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

EM_JS(void, uploadFile, (), {
  let input = document.getElementById('fileInput');
  if (!input) {
    input = document.createElement('input');
    input.type = 'file';
    input.accept = 'image/png, image/jpeg, image/gif';
    input.multiple = true;
    input.id = 'fileInput';
    input.style.display = 'none';
    document.body.appendChild(input);

    input.addEventListener('change', function(e) {
      const files = e.target.files;
      if (!files || files.length === 0) return;

      try { FS.mkdir('/assets'); } catch(e) {}

      for (const file of files) {

        if (file.size > 2 * 1024 * 1024)
        {
            console.warn("{file.name} is too large and will be skipped.");
            continue;
        }

        const reader = new FileReader();

        reader.onload = function(ev) {
            const data = new Uint8Array(ev.target.result);
            const filename = '/assets/' + file.name;

            FS.writeFile(filename, data);
            console.log('File written to ' + filename);

            const lengthBytes = lengthBytesUTF8(filename) + 1;
            const stringOnWasmHeap = _malloc(lengthBytes);
            stringToUTF8(filename, stringOnWasmHeap, lengthBytes);
            
        
            const texID = Module.ccall(
                'loadTextureFromFile',
                'number',
                ['number'],
                [stringOnWasmHeap]
                );

                console.log('Texture loaded with ID: ', texID);
                _free(stringOnWasmHeap);
                };

                reader.readAsArrayBuffer(file);
            };
        });
      }

      input.value = '';
      input.click();
});

EM_JS(void, uploadBase64Image, (const char* base64Str, const char* filenameCStr), {
    // Convert C strings to JS strings
    let base64 = UTF8ToString(base64Str);
    let filename = UTF8ToString(filenameCStr);

    // Remove data URL prefix if present
    if (base64.startsWith('data:')) {
        base64 = base64.split(',')[1];
    }

    // Decode Base64 to binary
    const binaryString = atob(base64);
    const len = binaryString.length;
    const bytes = new Uint8Array(len);
    for (let i = 0; i < len; i++) {
        bytes[i] = binaryString.charCodeAt(i);
    }

    // Write to Emscripten FS
    try { FS.mkdir('/assets'); } catch(e) {}
    const filepath = '/assets/' + filename;
    FS.writeFile(filepath, bytes);
    console.log('File written to ' + filepath);

    // Call your C++ function
    const lengthBytes = lengthBytesUTF8(filepath) + 1;
    const stringOnWasmHeap = _malloc(lengthBytes);
    stringToUTF8(filepath, stringOnWasmHeap, lengthBytes);

    const texID = Module.ccall(
        'loadTextureFromFile',  // C++ function name
        'number',               // return type
        ['number'],             // argument types
        [stringOnWasmHeap]      // arguments
    );

    console.log('Texture loaded with ID:', texID);
    _free(stringOnWasmHeap);
});

// This breaks the 10 commandments given by God. I hope the almighty father may forgive me.
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    GLuint loadTextureFromFile(const char* filename)
    {
        std::string fname(filename);
        fname.erase(fname.begin());
        tempNames.push_back(fname);

        int width, height, nrChannels;

        unsigned char* texData = stbi_load(filename, &width, &height, &nrChannels, 4);

        auto newTexture = std::make_shared<Texture>(width, height, 4, texData);
        tempTextures.push_back(newTexture);

        stbi_image_free(texData);

        return newTexture->getTextureID();
    }
}
// END EM_JS WIZARDRY (i fucking hate webdev)

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    ImGuiIO& io = ImGui::GetIO();
    float dpi = emscripten_get_device_pixel_ratio();

    io.DisplaySize = ImVec2((float)width, (float)height);
    io.DisplayFramebufferScale = ImVec2(dpi, dpi);
}

const char* vertex_shader = R"(#version 300 es
        
        layout(location = 0) in vec3 a_position;
        layout(location = 1) in vec2 aTexCoord;
        layout(location = 2) in vec4 a_color;
        
        uniform mat4 u_mvpMatrix;

        out vec4 v_color;
        out vec2 vTexCoord;

        void main() {
            gl_Position = u_mvpMatrix * vec4(a_position, 1.0);
            vTexCoord = aTexCoord;
            v_color = a_color;
        }
    )";

const char* vertex_shader2 = R"(#version 300 es
        
        layout(location = 0) in vec3 a_position;
        layout(location = 1) in vec2 a_TexCoord;
        layout(location = 2) in vec4 a_color;

        uniform mat4 u_modelMatrix;
        uniform mat4 u_viewMatrix;
        uniform mat4 u_projMatrix;
        uniform vec2 u_scale;

        out vec2 vTexCoord;
        out vec4 v_color;

        void main() {
            vec3 scaledPosition = vec3(a_position.x * u_scale.x, a_position.y * u_scale.y, a_position.z);
            gl_Position = u_modelMatrix * u_viewMatrix * u_projMatrix * vec4(scaledPosition, 1.0);
            vTexCoord = aTexCoord;
            v_color = a_color;
        }
    )";

const char* fragment_shader = R"(#version 300 es
        precision mediump float;

        uniform vec4 u_color;
        uniform sampler2D uTexture;
        uniform int usingSampler;
        
        in vec4 v_color;
        in vec2 vTexCoord;
        out vec4 outputColor;
        
        void main() {
        if(usingSampler == 1) {
            outputColor = (texture(uTexture, vTexCoord) * v_color);
         }
        else {
            outputColor = u_color;
         }
        }
    )";

const char* fragment_shader2 = R"(#version 300 es
        precision mediump float;

        uniform vec4 u_color;
        uniform sampler2D uTexture;
        uniform int usingSampler;
        
        in vec2 vTexCoord;
        in vec4 v_color;
        out vec4 outputColor;
        
        void main() {
        if(usingSampler == 1) {
            outputColor = texture(uTexture, vTexCoord);
         }
        else {
            outputColor = u_color + v_color;
         }
        }
    )";

Renderer::Renderer(int width, int height, const char* title)
 : lineVao(0), lineVbo(0)
{
    setTitle(title);
    setWidth(width);
    setHeight(height);

    if (!glfwInit())
        return;

    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!window)
    {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    emscripten_set_resize_callback(
    EMSCRIPTEN_EVENT_TARGET_WINDOW,
    this,
    false,
    [](int eventType, const EmscriptenUiEvent* e, void* userData) {
        Renderer* renderer = (Renderer*)userData;

        int newWidth = (int)e->windowInnerWidth;
        int newHeight = (int)e->windowInnerHeight;

        renderer->setWidth(newWidth);
        renderer->setHeight(newHeight);

        glfwSetWindowSize(renderer->getWindow(), newWidth, newHeight);

        framebuffer_size_callback(renderer->getWindow(), newWidth, newHeight);
        
        glm::mat4 newProjMat = glm::ortho(
            -newWidth / 2.0f, newWidth / 2.0f,
            -newHeight / 2.0f, newHeight / 2.0f,
            3.1f, -100.f
        );

        renderer->getCamera()->setProjectionMatrix(newProjMat);

        return EM_TRUE;
    });

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    const float dVal = 0.004f;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(dVal * 90, dVal * 90, dVal * 90, 1.0f);

    style.Colors[ImGuiCol_Button] = ImVec4(dVal * 70, dVal * 70, dVal * 70, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(dVal * 90, dVal * 90, dVal * 90, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(dVal * 90, dVal * 90, dVal * 90, 1.0f);
    
    style.FrameBorderSize = 1.0f;
    style.Colors[ImGuiCol_Border] = ImVec4(dVal * 90, dVal * 90, dVal * 90, 1.0f);

    style.Colors[ImGuiCol_WindowBg] = ImVec4(dVal * 40, dVal * 40, dVal * 40, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(dVal * 30, dVal * 30, dVal * 30, 1.0f);

    style.Colors[ImGuiCol_TitleBg] = ImVec4(dVal * 40, dVal * 40, dVal * 40, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(dVal * 40, dVal * 40, dVal * 40, 1.0f);

    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(dVal * 90, dVal * 90, dVal * 90, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(dVal * 90, dVal * 90, dVal * 90, 1.0f);

    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(dVal * 40, dVal * 40, dVal * 40, 1.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(dVal * 30, dVal * 30, dVal * 30, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(dVal * 70, dVal * 70, dVal * 70, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(dVal * 90, dVal * 90, dVal * 90, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(dVal * 90, dVal * 90, dVal * 90, 1.0f);


    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    shader = new Shader(vertex_shader, fragment_shader);
    camera = new Camera(-width/2.0f, width/2.0f, -height/2.0f, (height/2.0f)); //projection centered, so map appears relatively centered
    sb     = new SpriteBatch();
}

Renderer::~Renderer() 
{
    if (!window) return;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    emscripten_glfw_make_canvas_resizable(window, "window", nullptr);

    delete shader;
    shader = 0;

    delete camera;
    camera = 0;
}

void Renderer::onBeginFrame()
{
    safeToImport = false;
    shader->bind();
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::onEndFrame()
{
    sb->render(shader, camera);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
    safeToImport = true;
}

void Renderer::setTextures()
{
    for (size_t i = 0; i < tempTextures.size(); i++)
    {
        m_textures.push_back(tempTextures[i]);
    }

    for (size_t j = 0; j < tempNames.size(); j++)
    {
        m_fileNames.push_back(tempNames[j]);
    }

    tempTextures.clear();
    tempNames.clear();
}

void Renderer::resetTextures()
{
    tempTextures.clear();
    tempNames.clear(),
    m_fileNames.clear();
    m_textures.clear();
}

void Renderer::removeTexture(int index)
{
    m_fileNames.erase(m_fileNames.begin() + index);
    m_textures.erase(m_textures.begin() + index);
}

void Renderer::drawLine(glm::vec2& start, glm::vec2& end, const glm::vec4& color) {
    sb->addLine(start, end, color);
}

void Renderer::drawTile(glm::ivec2& pos, glm::ivec2& size, unsigned int texId, int zLayer, const glm::vec4& color) 
{
    glm::ivec2 pixelCoord = {pos.x * size.x, pos.y * size.y};

    if (texId != -1)
    {
        GLuint texture = m_textures[texId]->getTextureID();

        sb->addSprite(texture, pixelCoord, size, zLayer);
    }
}

void Renderer::setGridColor(const glm::vec4& color) {
    sb->setLineColor(color);
}

void Renderer::loadB64Texture(std::string name, std::string b64)
{
    uploadBase64Image(b64.c_str(), name.c_str());
}