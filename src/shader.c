#include "shader.h"

static uint32_t Shader_vertexCreate(const char* filepath)
{
    int params;
    char infoLog[INFO_LOG_LENGTH];

    const char* vss = read_file_to_buffer(filepath);
    uint32_t vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs,1,&vss,NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &params);
    if (!params) {
        glGetShaderInfoLog(vs, INFO_LOG_LENGTH, NULL, infoLog);
        Log(RED, "SHADER::VERTEX::ERROR: %s\n", infoLog);
        return SHADER_ERROR;
    }
    free((void*)vss);
    return vs;
}

static uint32_t Shader_fragmentCreate(const char* filepath)
{
    int params;
    char infoLog[INFO_LOG_LENGTH];

    const char* fss = read_file_to_buffer(filepath);
    uint32_t fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs,1,&fss,NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &params);
    if (!params) {
        glGetShaderInfoLog(fs, INFO_LOG_LENGTH, NULL, infoLog);
        Log(RED, "SHADER::FRAGMENT::ERROR: %s\n", infoLog);
        return SHADER_ERROR;
    }
    free((void*)fss);
    return fs;
}

static uint32_t Shader_programCreate(uint32_t vs, uint32_t fs)
{
    int params;
    char infoLog[INFO_LOG_LENGTH];
 
    uint32_t program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &params);
    if (!params) {
        glGetProgramInfoLog(program, INFO_LOG_LENGTH, NULL, infoLog);
        Log(RED, "SHADER::PROGRAM::ERROR: %s\n", infoLog);
        return SHADER_ERROR;
    }

    return program;
}

uint32_t Shader_create(const char* vs_path, const char* fs_path)
{
    uint32_t vs = Shader_vertexCreate(vs_path);

    uint32_t fs = Shader_fragmentCreate(fs_path);

    uint32_t program = Shader_programCreate(vs, fs);

    if (vs == SHADER_ERROR || fs == SHADER_ERROR || program == SHADER_ERROR) return SHADER_ERROR;

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void Shader_use(uint32_t program)
{
    glUseProgram(program);
}

void Shader_unbind(uint32_t program)
{
    glUseProgram(0);
}

void Shader_destroy(uint32_t program)
{
    glDeleteProgram(program);
}
