// input.h
#ifndef _INPUT_H_
#define _INPUT_H_

#include "main.h"
#include "window.h"

uint32_t Input_keyboardPressed(Window* window, int key);
uint32_t Input_keyboardReleased(Window* window, int key);
uint32_t Input_mousePressed(Window* window, int key);
uint32_t Input_mouseReleased(Window* window, int key);

#endif
