//
// Created by gregorym on 5/11/26.
//

#ifndef NOVA_ENGINE_ENGINE_H
#define NOVA_ENGINE_ENGINE_H

#define NOVA_VERSION_MAJOR 1
#define NOVA_VERSION_MINOR 0

#include <GLFW/glfw3.h>

#include "entity/entity.h"

extern Entity *             gp_entity;
void         run_engine(void);
GLFWwindow * get_window_handle(void);
int          get_window_width(void);
int          get_window_height(void);
float        get_aspect_ratio(void);
double       get_fps(void);
void         on_resize(GLFWwindow * p_window, int width, int height);
void         on_key(GLFWwindow * p_window, int key, int scancode, int action, int mods);
void         on_mouse_press(GLFWwindow * p_window, int button, int action, int mods);
void         on_cursor_move(GLFWwindow * p_window, int x, int y);
void         on_scroll(GLFWwindow * p_window, double xoffset, double yoffset);

#endif // NOVA_ENGINE_ENGINE_H
