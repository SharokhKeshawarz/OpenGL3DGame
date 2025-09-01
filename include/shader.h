#ifndef _SHADER_H_
#define _SHADER_H_

#include "main.h"
#include "utils.h"

#define INFO_LOG_LENGTH 1024
#define SHADER_ERROR 42

uint32_t Shader_create(const char* vs_path, const char* fs_path);
void Shader_use(uint32_t program);
void Shader_unbind(uint32_t program);
void Shader_destroy(uint32_t program);

#endif
