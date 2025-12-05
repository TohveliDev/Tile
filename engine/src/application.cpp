#include "application.h"

Application::Application(int width, int height, const char* title) : m_renderer(0)
{
    m_renderer = new Renderer(width, height, title);
    m_prevTime = (float)glfwGetTime();
}

void Application::run()
{
    m_isRunning = true;

    emscripten_set_main_loop_arg(
    [](void* arg) {
        Application* app = static_cast<Application*>(arg);
        app->mainLoop();
    }, this, 0, true);
}

void Application::mainLoop()
{
    if (!m_isRunning)
    {
        emscripten_cancel_main_loop();
        return;
    }

    float curTime = (float)glfwGetTime();
    float deltaTime = curTime - m_prevTime;
    m_prevTime = curTime;

    update(deltaTime);
    m_renderer->onBeginFrame();
    render();
    m_renderer->onEndFrame();
}