#pragma once
#include <glm/glm.hpp>
#include <GLFW/emscripten_glfw3.h>

class InputSystem
{
public:
    InputSystem(GLFWwindow* win);

    typedef void(*GLFWmousebuttonfun)   (GLFWwindow* window, int button, int action, int mods);
    typedef void(*GLFWcursorposfun)     (GLFWwindow* window, double xpos, double ypos);

    bool isButtonPressed(int button);
    glm::dvec2 getCursorCoordinates();

private:
    GLFWwindow* window;
};