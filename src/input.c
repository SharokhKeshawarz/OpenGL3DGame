#include "input.h"

uint32_t Input_keyboardPressed(Window* window, int key)
{
    return glfwGetKey(window->handle, key) == GLFW_PRESS;
}

uint32_t Input_keyboardReleased(Window* window, int key)
{
    return glfwGetKey(window->handle, key) == GLFW_RELEASE;
}

uint32_t Input_mousePressed(Window* window, int key)
{
    return glfwGetMouseButton(window->handle, key) == GLFW_PRESS;
}

uint32_t Input_mouseReleased(Window* window, int key)
{
    return glfwGetMouseButton(window->handle, key) == GLFW_RELEASE;
}
