#pragma once
//std
#include <memory>
//omat
#include "renderer.h"
#include "tilemap.h"

class Application
{
public:
    Application(int width, int height, const char* title);

    ~Application() { }

    void run();

    void mainLoop();

    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

    Renderer* getRenderer() { return m_renderer; }

private:
    Renderer* m_renderer;
    bool m_isRunning = false;
    float m_prevTime = 0.0f;
};