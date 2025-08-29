#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "fastobj/fast_obj.h"
#include "cglm/cglm.h"

#define DISABLE_VSYNC 0
#define ENABLE_VSYNC 1

typedef enum LogType { RED, GREEN } LogType;

void Log(LogType type, const char* format, ...) {
    va_list args;
    va_start(args, format);

    switch (type) {
        case RED: fprintf(stderr, "\033[31m"); break;
        case GREEN: fprintf(stderr, "\033[32m"); break;
        default: break;
    }

    vfprintf(stderr, format, args);
    fprintf(stderr, "\033[0m");
    va_end(args);
}

char* read_file_to_buffer(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    char *buffer;
    size_t size;
    if (!fp) { Log(RED, "Error opening file: %s\n", filename); return NULL; }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);

    buffer = (char*)malloc(sizeof(char) * (size + 1));
    if (!buffer) { Log(RED, "Memory allocation failed\n"); fclose(fp); return NULL; }

    fread(buffer, 1, size, fp);
    buffer[size] = '\0';
    fclose(fp);
    return buffer;
}

// ===================== Camera =====================
typedef struct {
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 worldUp;

    float yaw;
    float pitch;

    float speed;      // movement speed
    float fov;
    float aspect;
    float nearPlane;
    float farPlane;

    mat4 viewMatrix;
    mat4 projectionMatrix;
} Camera;

void Camera_updateView(Camera* cam) {
    vec3 target;
    glm_vec3_add(cam->position, cam->front, target);
    glm_lookat(cam->position, target, cam->worldUp, cam->viewMatrix);
}

void Camera_init(Camera* cam, vec3 position, vec3 target, vec3 up, float fov, float aspect, float nearPlane, float farPlane, float speed) {
    glm_vec3_copy(position, cam->position);
    glm_vec3_sub(target, position, cam->front);
    glm_vec3_normalize(cam->front);

    glm_vec3_copy(up, cam->worldUp);

    cam->yaw = -90.0f;
    cam->pitch = 0.0f;
    cam->speed = speed;

    cam->fov = fov;
    cam->aspect = aspect;
    cam->nearPlane = nearPlane;
    cam->farPlane = farPlane;

    glm_vec3_cross(cam->front, cam->worldUp, cam->right);
    glm_vec3_normalize(cam->right);
    glm_vec3_cross(cam->right, cam->front, cam->up);
    glm_vec3_normalize(cam->up);

    glm_perspective(glm_rad(fov), aspect, nearPlane, farPlane, cam->projectionMatrix);
    Camera_updateView(cam);
}

void Camera_rotate(Camera* cam, float yawOffset, float pitchOffset) {
    cam->yaw += yawOffset;
    cam->pitch += pitchOffset;

    if (cam->pitch > 89.0f) cam->pitch = 89.0f;
    if (cam->pitch < -89.0f) cam->pitch = -89.0f;

    cam->front[0] = cos(glm_rad(cam->yaw)) * cos(glm_rad(cam->pitch));
    cam->front[1] = sin(glm_rad(cam->pitch));
    cam->front[2] = sin(glm_rad(cam->yaw)) * cos(glm_rad(cam->pitch));
    glm_vec3_normalize(cam->front);

    glm_vec3_cross(cam->front, cam->worldUp, cam->right);
    glm_vec3_normalize(cam->right);
    glm_vec3_cross(cam->right, cam->front, cam->up);
    glm_vec3_normalize(cam->up);

    Camera_updateView(cam);
}

void Camera_processKeyboard(Camera* cam, int key, float deltaTime) {
    float velocity = cam->speed * deltaTime;
    vec3 temp;

    if (key == GLFW_KEY_W) { glm_vec3_scale(cam->front, velocity, temp); glm_vec3_add(cam->position, temp, cam->position); }
    if (key == GLFW_KEY_S) { glm_vec3_scale(cam->front, velocity, temp); glm_vec3_sub(cam->position, temp, cam->position); }
    if (key == GLFW_KEY_A) { glm_vec3_scale(cam->right, velocity, temp); glm_vec3_sub(cam->position, temp, cam->position); }
    if (key == GLFW_KEY_D) { glm_vec3_scale(cam->right, velocity, temp); glm_vec3_add(cam->position, temp, cam->position); }

    Camera_updateView(cam);
}

// ================= Mouse =================
Camera cam;
bool firstMouse = true;
double lastX, lastY;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    Camera_rotate(&cam, xoffset * 0.1f, yoffset * 0.1f);
}

// ================= Main =================
int main(void) {
    if (!glfwInit()) { Log(RED, "Failed to init GLFW\n"); return EXIT_FAILURE; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 800, height = 600;
    GLFWwindow* window = glfwCreateWindow(width, height, "Game", NULL, NULL);
    if (!window) { Log(RED, "Failed to init Window\n"); return EXIT_FAILURE; }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(DISABLE_VSYNC);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { Log(RED, "Failed to init GLAD\n"); return EXIT_FAILURE; }
    glViewport(0,0,width,height);
    glEnable(GL_DEPTH_TEST);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    fastObjMesh* mesh = fast_obj_read("models/cube.obj");
    if (!mesh) { Log(RED, "Failed to load mesh\n"); return EXIT_FAILURE; }

    const char* vss = read_file_to_buffer("shaders/default_vert.glsl");
    const char* fss = read_file_to_buffer("shaders/default_frag.glsl");

    uint32_t vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs,1,&vss,NULL); glCompileShader(vs);
    free((void*)vss);

    uint32_t fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs,1,&fss,NULL); glCompileShader(fs);
    free((void*)fss);

    uint32_t program = glCreateProgram();
    glAttachShader(program, vs); glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs); glDeleteShader(fs);
float* vertices = malloc(mesh->index_count * 3 * sizeof(float));
unsigned int* indices = malloc(mesh->index_count * sizeof(unsigned int));

for (size_t i = 0; i < mesh->index_count; i++) {
    indices[i] = i; // one-to-one

    size_t pi = mesh->indices[i].p;
    if (pi > 0) pi -= 1; // <-- crucial 1-based correction

    vertices[i*3+0] = mesh->positions[pi*3+0];
    vertices[i*3+1] = mesh->positions[pi*3+1];
    vertices[i*3+2] = mesh->positions[pi*3+2];
}



    uint32_t vao,vbo,ebo;
    glGenVertexArrays(1,&vao);
    glGenBuffers(1,&vbo);
    glGenBuffers(1,&ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,mesh->position_count*3*sizeof(float),vertices,GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,mesh->index_count*sizeof(unsigned int),indices,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    Camera_init(&cam,(vec3){0,0,5},(vec3){0,0,0},(vec3){0,1,0},45.0f,(float)width/height,0.1f,500.0f,20.0f);

    float lastFrame = 0.0f;

    while(!glfwWindowShouldClose(window)){
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        if(glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS) Camera_processKeyboard(&cam, GLFW_KEY_W, deltaTime);
        if(glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS) Camera_processKeyboard(&cam, GLFW_KEY_S, deltaTime);
        if(glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS) Camera_processKeyboard(&cam, GLFW_KEY_A, deltaTime);
        if(glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS) Camera_processKeyboard(&cam, GLFW_KEY_D, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)==GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);

        glClearColor(0.1f,0.2f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program);

        mat4 model;
        glm_mat4_identity(model);
        glm_translate(model, (vec3){0.0f, 0.0f, -2.0f});
        glm_scale(model, (vec3){0.2f, 0.2f, 0.2f});

        glUniformMatrix4fv(glGetUniformLocation(program,"projection"),1,GL_FALSE,&cam.projectionMatrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(program,"view"),1,GL_FALSE,&cam.viewMatrix[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(program,"model"),1,GL_FALSE,&model[0][0]);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES,mesh->index_count,GL_UNSIGNED_INT,0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    free(vertices);
    free(indices);
    fast_obj_destroy(mesh);
    glDeleteBuffers(1,&ebo);
    glDeleteBuffers(1,&vbo);
    glDeleteVertexArrays(1,&vao);
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
