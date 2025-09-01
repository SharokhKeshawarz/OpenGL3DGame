# Compiler
CC = gcc
CFLAGS = -Iinclude -Iother -Iother/tinyobjloader -Iother/glad -Iother/GLFW -Iother/fastobj -Iother/cglm -Wall -Wextra
LDFLAGS = -Lother/GLFW -lglfw3dll -lopengl32 -lgdi32

# Directories
SRC_DIR = src
OBJ_DIR = bin
TARGET = $(OBJ_DIR)/main.exe

# Gather all .c files in src and subdirectories, plus dependencies
SRCS := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c) \
        other/glad/glad.c other/fastobj/fast_obj.c other/tinyobjloader/tinyobj_loader.c 

# Map .c files to .o files in bin/
OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

# Default target
all: $(TARGET)
	./$(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile each .c file to .o in bin/ maintaining folder structure
$(OBJ_DIR)/%.o: %.c
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	del /Q $(OBJ_DIR)\*.o 2>nul
	del /Q $(TARGET) 2>nul
