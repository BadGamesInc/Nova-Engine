//
// Created by gregorym on 5/11/26.
//

#ifndef NOVA_ENGINE_WINDOW_H
#define NOVA_ENGINE_WINDOW_H

#include <GLFW/glfw3.h>
#include <stdint.h>

GLFWwindow * create_window (int32_t width, int32_t height, const char * p_title);
void         show_window (GLFWwindow * p_window);

#endif //NOVA_ENGINE_WINDOW_H
