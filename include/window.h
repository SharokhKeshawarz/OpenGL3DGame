// window.h
#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "main.h"
#include "utils.h"

typedef struct Window {
    uint32_t width, height;
    char* title;
    GLFWwindow* handle;
    struct clock {
        float lastFrame;
        float currentFrame;
        float deltaTime;
        int frameCount;
        double fpsTime;
    } clock;
} Window;

Window* Window_init(uint32_t width, uint32_t height, char* title);
void Window_destroy(Window* window);
void Window_update(Window* window);
void Window_clear(float r, float g, float b);
void Window_swapBuffer(Window* window);

GLFWwindow* Window_getHandle(Window* window);
uint32_t Window_getWidth(Window* window);
uint32_t Window_getHeight(Window* window);
char* Window_getTitle(Window* window);

#endif
