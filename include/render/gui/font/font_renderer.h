//
// Created by gregorym on 6/14/26.
//

#ifndef NOVA_ENGINE_FONT_H
#define NOVA_ENGINE_FONT_H
#include "mat4.h"
#include "freetype/freetype.h"
#include "util/containers.h"

typedef struct
{
    unsigned int  texture_id;
    ivec2         size;
    ivec2         bearing;
    unsigned int  advance;
} Glyph;

typedef struct
{
    FT_Face      font_face;
    HashMap *    glyph_map;
    unsigned int vao_id;
    unsigned int vbo_id;
} Font;

extern Font * gp_default_font;

void   font_renderer_init(mat4 projection_matrix);
Font * create_font(const char * p_font_file_name, int size);
float  font_get_string_width(const Font * p_font, const char * text, float scale);
float  font_get_string_height(const Font * p_font, const char * text, float scale);
void   destroy_font(Font * p_font);
void   font_renderer_draw_string(const Font * p_font, const char * p_text, float x, float y, float scale, vec3 color);
void   font_renderer_resize(mat4 projection_matrix);
void   font_renderer_cleanup(void);

#endif //NOVA_ENGINE_FONT_H
