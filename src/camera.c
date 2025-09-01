#include "camera.h"

Camera* Camera_init(Window* window, float speed, float sensitivity, float fov, float nearPlane, float farPlane)
{
    Camera* camera = malloc(sizeof(Camera));

    camera->firstClick = true;
    camera->width = Window_getWidth(window);
    camera->height = Window_getHeight(window);
    camera->speed = speed;
    camera->sensitivity = sensitivity;
    camera->fov = fov;
    camera->nearPlane = nearPlane;
    camera->farPlane = farPlane;
    camera->aspect = (float)((float)camera->width/(float)camera->height);
    
    camera->pitch = 0.0f;
    camera->yaw = -90.0f;

    glm_vec3_copy((vec3){0.0f, 0.0f, 5.0f}, camera->position);
    glm_vec3_copy((vec3){0.0f, 0.0f, -1.0f}, camera->orientation);
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, camera->up);
    glm_mat4_identity(camera->cameraMatrix);
    glm_mat4_identity(camera->viewMatrix);
    glm_mat4_identity(camera->projectionMatrix);

    return camera;
}

static void Camera_updateMatrix(Camera* camera)
{
    glm_mat4_identity(camera->viewMatrix);
    glm_mat4_identity(camera->projectionMatrix);

    vec3 center;
    glm_vec3_add(camera->position, camera->orientation, center);
    glm_lookat(camera->position, center, camera->up, camera->viewMatrix);

    glm_perspective(glm_rad(camera->fov), camera->aspect ,camera->nearPlane, camera->farPlane, camera->projectionMatrix);

    glm_mat4_mul(camera->projectionMatrix, camera->viewMatrix, camera->cameraMatrix);
}

void Camera_getViewMatrix(uint32_t program, const char* uniform, Camera* camera)
{
    glUniformMatrix4fv(glGetUniformLocation(program, uniform), 1, GL_FALSE, &camera->viewMatrix[0][0]);
}

void Camera_getProjectionMatrix(uint32_t program, const char* uniform, Camera* camera)
{
    glUniformMatrix4fv(glGetUniformLocation(program, uniform), 1, GL_FALSE, &camera->projectionMatrix[0][0]);
}

void Camera_matrix(uint32_t program, const char* uniform, Camera* camera)
{
    glUniformMatrix4fv(glGetUniformLocation(program, uniform), 1, GL_FALSE, &camera->cameraMatrix[0][0]);
}

static void Camera_inputKeyboard(Window* window, Camera* camera)
{
    float new_speed = window->clock.deltaTime * camera->speed;
    vec3 right;
    glm_vec3_cross(camera->orientation, camera->up, right);
    glm_vec3_normalize(right);

    if (Input_keyboardPressed(window, GLFW_KEY_W))
        glm_vec3_muladds(camera->orientation, new_speed, camera->position);
    if (Input_keyboardPressed(window, GLFW_KEY_S))
        glm_vec3_muladds(camera->orientation, -new_speed, camera->position);
    if (Input_keyboardPressed(window, GLFW_KEY_A))
        glm_vec3_muladds(right, -new_speed, camera->position);
    if (Input_keyboardPressed(window, GLFW_KEY_D))
        glm_vec3_muladds(right, new_speed, camera->position);

    if (Input_keyboardPressed(window, GLFW_KEY_SPACE))
        glm_vec3_muladds(camera->up, new_speed, camera->position);
    
    if (Input_keyboardPressed(window, GLFW_KEY_LEFT_CONTROL))
        glm_vec3_muladds(camera->up, -new_speed, camera->position);

    if (Input_keyboardPressed(window, GLFW_KEY_LEFT_SHIFT))
        camera->speed = 50;
    if (Input_keyboardReleased(window, GLFW_KEY_LEFT_SHIFT))
        camera->speed = 25;
}

static void Camera_inputMouse(Window* window, Camera* camera)
{
    if (Input_mousePressed(window, GLFW_MOUSE_BUTTON_LEFT))
    {
        glfwSetInputMode(window->handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        if (camera->firstClick) {
            glfwSetCursorPos(window->handle, Window_getWidth(window) / 2, Window_getHeight(window) / 2);
            camera->firstClick = false;
        }

        double mouseX, mouseY;
        glfwGetCursorPos(window->handle, &mouseX, &mouseY);

        float rotX = camera->sensitivity * ((Window_getHeight(window)/2) - (float)mouseY) / Window_getHeight(window);
        float rotY = camera->sensitivity * ((float)mouseX - Window_getWidth(window)/2) / Window_getWidth(window);

        camera->yaw   += rotY;
        camera->pitch += rotX;

        if(camera->pitch > 89.0f) camera->pitch = 89.0f;
        if(camera->pitch < -89.0f) camera->pitch = -89.0f;

        camera->orientation[0] = cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
        camera->orientation[1] = sinf(glm_rad(camera->pitch));
        camera->orientation[2] = sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
        glm_vec3_normalize(camera->orientation);

        glfwSetCursorPos(window->handle, Window_getWidth(window) / 2, Window_getHeight(window) / 2);
    }
    else if (Input_keyboardReleased(window, GLFW_MOUSE_BUTTON_LEFT))
    {
        glfwSetInputMode(window->handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        camera->firstClick = true;
    }
}

void Camera_update(Window* window, Camera* camera)
{
    Camera_inputKeyboard(window, camera);
    Camera_inputMouse(window, camera);
    Camera_updateMatrix(camera);
}
