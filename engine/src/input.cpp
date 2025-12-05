#include "input.h"

InputSystem::InputSystem(GLFWwindow* win) : window(win)
{
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
}

bool InputSystem::isButtonPressed(int button)
{
    int state = glfwGetMouseButton(window, button);

    if (state == GLFW_PRESS)
    {
        return true;
    }

    return false;
}

glm::dvec2 InputSystem::getCursorCoordinates()
{
    double cursorX, cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);
    return glm::dvec2(cursorX, cursorY);
}