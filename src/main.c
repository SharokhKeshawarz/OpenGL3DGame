#include "window.h"
#include "fastobj/fast_obj.h"
#include "cglm/cglm.h"
#include "utils.h"
#include "input.h"
#include "shader.h"
#include "camera.h"

int main(void)
{
    Window* window = Window_init(800, 600, "Game");
    if (window == NULL) {
        Log(RED, "Failed to init window\n");
        return EXIT_FAILURE;
    }

    uint32_t program = Shader_create("shaders/default_vert.glsl", "shaders/default_frag.glsl");
    if (program == SHADER_ERROR) {
        Log(RED, "Failed to init program\n");
        return EXIT_FAILURE;
    }

    float veritces[] = {
        -0.5f, -0.5f, +0.0f,
        +0.5f, -0.5f, +0.0f,
        +0.5f, +0.5f, +0.0f,
        -0.5f, +0.5f, +0.0f,
    };

    uint32_t indices[] = {
        0, 1, 2,
        2, 3, 0,
    };

    uint32_t vao,vbo,ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(veritces), veritces, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    Camera* camera = Camera_init(window, 25.0f, 100.0f, 45.0f, 0.1f, 100.0f);

    while(!glfwWindowShouldClose(window->handle)){
        Window_update(window);
        Camera_update(window, camera);
        
        Window_clear(0.1f, 0.2f, 0.3f);

        Shader_use(program);
        Camera_matrix(program, "cameraMatrix", camera);
        
        mat4 model;
        glm_mat4_identity(model);
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &model[0][0]);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(indices[0]), GL_UNSIGNED_INT, NULL);
        glBindVertexArray(0);
        
        Window_swapBuffer(window);
    }
    
    Shader_destroy(program);
    Window_destroy(window);
    return EXIT_SUCCESS;
}
