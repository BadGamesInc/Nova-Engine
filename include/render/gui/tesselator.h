//
// Created by gregorym on 6/14/26.
//

#ifndef NOVA_ENGINE_TESSELATOR_H
#define NOVA_ENGINE_TESSELATOR_H

#define TESSELATOR_DEFAULT_BUFFER_SIZE 2097152
#include <stdbool.h>

void tesselator_init(void);
void tesselator_begin(int mode);
void tesselator_begin_formatted(int mode, bool color, bool texture);
void tesselator_fcolor(float r, float g, float b, float a);
void tesselator_color(int r, int g, int b, int a);
void tesselator_tex_coord(float u, float v);
void tesselator_vertex(float x, float y);
void tesselator_vertex_texture(float x, float y, float u, float v);
void tesselator_end(void);
void tesselator_cleanup(void);

#endif // NOVA_ENGINE_TESSELATOR_H
