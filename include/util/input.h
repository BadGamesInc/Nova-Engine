//
// Created by gregorym on 6/7/26.
//

#ifndef NOVA_ENGINE_INPUT_H
#define NOVA_ENGINE_INPUT_H

#include <stdbool.h>

bool   is_key_pressed(int key);
bool   is_mouse_button_pressed(int button);
void   update_cursor_pos(void);
void   update_mouse_scroll(double xoffset, double yoffset);
double get_mouse_x(void);
double get_mouse_y(void);
double get_mouse_scroll_x(void);
double get_mouse_scroll_y(void);
void   input_cleanup(void);

#endif //NOVA_ENGINE_INPUT_H
