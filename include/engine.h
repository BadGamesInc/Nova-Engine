//
// Created by gregorym on 5/11/26.
//

#ifndef NOVA_ENGINE_ENGINE_H
#define NOVA_ENGINE_ENGINE_H

#define NOVA_VERSION_MAJOR 1
#define NOVA_VERSION_MINOR 0

#include <stdint.h>
#include <GLFW/glfw3.h>

void    run_engine (void);
int32_t get_window_width (void);
int32_t get_window_height (void);
float   get_aspect_ratio (void);
void    on_resize (GLFWwindow * p_window, int32_t width, int32_t height);
void    on_key (GLFWwindow * p_window, int32_t key, int32_t scancode, int32_t action, int32_t mods);
void    on_mouse_press (GLFWwindow * p_window, int32_t button, int32_t action, int32_t mods);
void    on_cursor_move (GLFWwindow * p_window, int32_t x, int32_t y);
void    on_scroll (GLFWwindow * p_window, double xoffset, double yoffset);

#endif //NOVA_ENGINE_ENGINE_H
