#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "main.h"
#include "shader.h"
#include "input.h"
#include "window.h"

typedef struct Camera
{
    bool firstClick;
    int width, height;
    float aspect;
    float speed;
    float sensitivity;
    float fov;
    float nearPlane, farPlane;

    float yaw;
    float pitch;

    vec3 position;
    vec3 orientation;
    vec3 up;

    mat4 cameraMatrix;
    mat4 viewMatrix;
    mat4 projectionMatrix;
} Camera;

Camera* Camera_init(Window* window, float speed, float sensitivity, float fov, float nearPlane, float farPlane);
void Camera_getViewMatrix(uint32_t program, const char* uniform, Camera* camera);
void Camera_getProjectionMatrix(uint32_t program, const char* uniform, Camera* camera);
void Camera_matrix(uint32_t program, const char* uniform, Camera* camera);
//void Camera_updateMatrix(Camera* camera);
// void Camera_inputKeyboard(Window* window, Camera* camera);
// void Camera_inputMouse(Window* window, Camera* camera);
void Camera_update(Window* window, Camera* camera);

#endif
