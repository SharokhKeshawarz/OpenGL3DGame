#include "utils.h"

void Log(LogType type, const char* format, ...)
{
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

char* read_file_to_buffer(const char *filename)
{
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
