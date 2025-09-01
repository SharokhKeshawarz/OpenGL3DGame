#include "window.h"

static void init_glfw()
{
    if (!glfwInit()) { 
        Log(RED, "Failed to init GLFW\n"); 
        return; 
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

static void init_glad(uint32_t width, uint32_t height)
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { 
        Log(RED, "Failed to init GLAD\n"); 
        return;
    }
    glViewport(0,0,width,height);
    glEnable(GL_DEPTH_TEST);
}

static void init_settings(Window* window)
{
    glfwMakeContextCurrent(window->handle);
    glfwSwapInterval(DISABLE_VSYNC);
}

Window* Window_init(uint32_t width, uint32_t height, char* title)
{
    init_glfw();

    Window* window = malloc(sizeof(Window));
    if (window == NULL) {
        Log(RED, "Failed to init window struct\n");
        return NULL;
    }

    window->handle = glfwCreateWindow(width, height, "Game", NULL, NULL);
    if (!window->handle) { 
        Log(RED, "Failed to init Window\n"); 
        return NULL; 
    }  
    window->clock.lastFrame = 0.0f;
    window->clock.fpsTime = 0.0f;
    window->clock.frameCount = 0;
    window->width = width;
    window->height = height;
    window->title = strdup(title);
    if (window->title == NULL) {
        Log(RED, "Failed to init window title\n");
        return NULL;
    }
    
    init_settings(window);

    init_glad(width, height);

    return window;
};

GLFWwindow* Window_getHandle(Window* window)
{
    return window->handle;
}

uint32_t Window_getWidth(Window* window)
{
    return window->width;
}

uint32_t Window_getHeight(Window* window)
{
    return window->height;
}

char* Window_getTitle(Window* window)
{
    return window->title;
}

void Window_update(Window* window)
{
    window->clock.currentFrame = glfwGetTime();
    window->clock.deltaTime = window->clock.currentFrame - window->clock.lastFrame;
    window->clock.lastFrame = window->clock.currentFrame;

    window->clock.frameCount++;
    if (window->clock.currentFrame - window->clock.fpsTime >= 1.0f) {
        int fps = window->clock.frameCount;
        window->clock.frameCount = 0;
        window->clock.fpsTime = window->clock.currentFrame;

        char title[128];
        snprintf(title, sizeof(title), "%s - FPS: %d", window->title, fps);
        glfwSetWindowTitle(window->handle, title);
    }
}

void Window_clear(float r, float g, float b)
{
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window_swapBuffer(Window* window)
{
    glfwSwapBuffers(window->handle);
    glfwPollEvents();
    if (glfwGetKey(window->handle, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window->handle, GLFW_TRUE);
}

void Window_destroy(Window* window)
{
    window->width = 0;
    window->height = 0;
    free(window->title);
    glfwDestroyWindow(window->handle);
    free(window);
    glfwTerminate();
}
