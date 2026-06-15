//
// Created by gregorym on 6/14/26.
//

#ifndef NOVA_ENGINE_GUI_RENDERER_H
#define NOVA_ENGINE_GUI_RENDERER_H

#include "ivec2.h"
#include "font/font_renderer.h"

void gui_renderer_init(void);
void gui_renderer_render(void);
void gui_renderer_draw_string(const Font * p_font, const char * p_text, float x, float y, float scale, vec3 color);
void gui_renderer_draw_rect(float x, float y, float width, float height, vec4 color);
void gui_renderer_draw_rect_outline(float x, float y, float width, float height, float line_width, vec4 color);
void gui_renderer_draw_gradient_rect(float x, float y, float width, float height, vec4 color1, vec4 color2);
void gui_renderer_draw_textured_rect(float x, float y, float width, float height);
void gui_renderer_draw_textured_rect_uv(float x, float y, float width, float height, float u0, float v0, float u1, float v1);
void gui_renderer_draw_textured_rect_colored(float x, float y, float width, float height, float u0, float v0, float u1, float v1, vec4 color);
void gui_renderer_draw_line(float x1, float y1, float x2, float y2, float line_width, vec4 color);
void gui_renderer_draw_circle(float cx, float cy, float radius, int segments, vec4 color);
void gui_renderer_resize(int width, int height);
void gui_renderer_cleanup(void);

#endif //NOVA_ENGINE_GUI_RENDERER_H
