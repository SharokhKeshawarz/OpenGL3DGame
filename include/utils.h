#ifndef _UTILS_H_
#define _UTILS_H_

#include "main.h"

#define DISABLE_VSYNC 0
#define ENABLE_VSYNC 1

typedef enum LogType { RED, GREEN } LogType;
char* read_file_to_buffer(const char *filename);
void Log(LogType type, const char* format, ...);

#endif
