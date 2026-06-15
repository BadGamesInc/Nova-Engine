//
// Created by gregorym on 6/14/26.
//

#include "render/gui/font/font_renderer.h"

#include <gl.h>

#include "mat4.h"
#include "freetype/ftmodapi.h"
#include "render/shader.h"
#include "util/fileutils.h"
#include "util/nova_logger.h"

static FT_Library   gp_ft = {0};
static mat4         g_projection_matrix;
static FontShader * gp_font_shader = NULL;

Font * gp_default_font = NULL;

void font_renderer_init(mat4 projection_matrix)
{
    if (FT_Init_FreeType(&gp_ft) != FT_Err_Ok)
    {
        nova_error("Failed to initialize FreeType");
        return;
    }

    int ver_major = 0;
    int ver_minor = 0;
    int patch     = 0;
    FT_Library_Version(gp_ft, &ver_major, &ver_minor, &patch);
    nova_info("Loaded FreeType %d.%d.%d", ver_major, ver_minor, patch);

    glm_mat4_copy(projection_matrix, g_projection_matrix);
    gp_font_shader = create_font_shader();
    bind_shader((Shader *)gp_font_shader);
    shader_uniform_mat4((Shader *)gp_font_shader, "projection", projection_matrix);
    unbind_shader();

    gp_default_font = create_font("roboto.ttf", 50);
}

static void load_char(const Font * p_font, const char c)
{
    const int error = FT_Load_Char(p_font->font_face, c, FT_LOAD_RENDER);
    if (error != FT_Err_Ok)
    {
        nova_error("Failed to load glyph for character %c: %s", c, FT_Error_String(error));
    }
}

static void load_chars(const Font * p_font, const int limit)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (int c = 0; c < limit; c++)
    {
        load_char(p_font, (char) c);
        unsigned int texture_id = 0;
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        const FT_Bitmap bitmap = p_font->font_face->glyph->bitmap;
        const unsigned int width = bitmap.width;
        const unsigned int rows = bitmap.rows;

        if (width > 0 && rows > 0)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (int) width, (int) rows, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (int) width, (int) rows, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        Glyph * glyph = malloc(sizeof(*glyph));
        if (glyph == NULL)
        {
            nova_error("Failed to allocate Glyph");
            continue;
        }
        glyph->texture_id = texture_id;
        glm_ivec2_copy((ivec2) {(int) width, (int) rows}, glyph->size);
        glm_ivec2_copy((ivec2) {p_font->font_face->glyph->bitmap_left, p_font->font_face->glyph->bitmap_top}, glyph->bearing);
        glyph->advance = p_font->font_face->glyph->advance.x;
        const char character = (char) c;
        hashmap_put(p_font->glyph_map, char_dup(character), glyph);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

static void init_vao(Font * p_font)
{
    glGenVertexArrays(1, &p_font->vao_id);
    glGenBuffers(1, &p_font->vbo_id);
    glBindVertexArray(p_font->vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, p_font->vbo_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static void set_font_size(const Font * p_font, const int size)
{
    const int error = FT_Set_Pixel_Sizes(p_font->font_face, 0, size);
    if (error != FT_Err_Ok)
    {
        nova_error("Failed to set font size: %s", FT_Error_String(error));
    }
}

Font * create_font(const char * p_font_file_name, const int size)
{
    Font * p_font = malloc(sizeof(*p_font));

    if (p_font == NULL)
    {
        nova_error("Failed to allocate Font");
        return NULL;
    }

    const int error = FT_New_Face(gp_ft, get_resource_location("fonts", p_font_file_name), 0, &p_font->font_face);
    if (error != FT_Err_Ok)
    {
        nova_error("Failed to load font %s: %s", p_font_file_name, FT_Error_String(error));
    }
    p_font->glyph_map = create_hashmap(128, char_hash, container_char_compare, STRUCT_COMPARE_FUNC(Glyph, texture_id));
    set_font_size(p_font, size);
    load_chars(p_font, 128);
    init_vao(p_font);
    nova_info("Loaded font %s", p_font_file_name);

    return p_font;
}

float font_get_string_width(const Font * p_font, const char * text, const float scale)
{
    float width = 0;
    for (int i = 0; i < (int) strlen(text); i++)
    {
        const char c = text[i];
        if (c < 0)
        {
            continue;
        }
        const Glyph * p_glyph = hashmap_get(p_font->glyph_map, &c);
        if (p_glyph == NULL)
        {
            continue;
        }
        width += (float) (p_glyph->advance >> 6) * scale;
    }

    return width;
}

float font_get_string_height(const Font * p_font, const char * text, const float scale)
{
    float max_height = 0;
    for (int i = 0; i < (int) strlen(text); i++)
    {
        const char c = text[i];
        if (c < 0)
        {
            continue;
        }
        const Glyph * p_glyph = hashmap_get(p_font->glyph_map, &c);
        if (p_glyph == NULL)
        {
            continue;
        }
        const float height = (float) p_glyph->bearing[1] * scale;
        if (height > max_height)
        {
            max_height = height;
        }
    }

    return max_height;
}

static void delete_glyph(void * p_glyph)
{
    Glyph * glyph = p_glyph;
    glDeleteTextures(1, &glyph->texture_id);
    free(glyph);
}

void destroy_font(Font * p_font)
{
    free_hashmap(p_font->glyph_map, free, delete_glyph);
    glDeleteVertexArrays(1, &p_font->vao_id);
    glDeleteBuffers(1, &p_font->vbo_id);
    FT_Done_Face(p_font->font_face);
    free(p_font);
}

static void prepare(void)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void font_renderer_draw_string(const Font * p_font, const char * p_text, float x, float y, float scale, vec3 color)
{
    prepare();
    bind_shader((Shader *)gp_font_shader);
    shader_uniform_mat4((Shader *)gp_font_shader, "projection", g_projection_matrix);
    shader_uniform_3f((Shader *)gp_font_shader, "text_color", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(p_font->vao_id);

    for (int i = 0; i < (int) strlen(p_text); i++)
    {
        const char c = p_text[i];
        if (c < 0)
        {
            continue;
        }
        const Glyph * p_glyph = hashmap_get(p_font->glyph_map, &c);
        const float xpos = x + (float) p_glyph->bearing[0] * scale;
        const float ypos = (y - ((float) p_glyph->bearing[1]) * scale) + font_get_string_height(p_font, p_text, scale);
        const float width = (float) p_glyph->size[0] * scale;
        const float height = (float) p_glyph->size[1] * scale;
        const float vertices[24] = {
            xpos,         ypos,          0.0f, 0.0f,
            xpos,         ypos + height, 0.0f, 1.0f,
            xpos + width, ypos + height, 1.0f, 1.0f,
            xpos,         ypos,          0.0f, 0.0f,
            xpos + width, ypos + height, 1.0f, 1.0f,
            xpos + width, ypos,          1.0f, 0.0f
        };

        glBindTexture(GL_TEXTURE_2D, p_glyph->texture_id);
        glBindBuffer(GL_ARRAY_BUFFER, p_font->vbo_id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (float) (p_glyph->advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    unbind_shader();
}

void font_renderer_resize(mat4 projection_matrix)
{
    glm_mat4_copy(projection_matrix, g_projection_matrix);
    bind_shader((Shader *)gp_font_shader);
    shader_uniform_mat4((Shader *)gp_font_shader, "projection", g_projection_matrix);
    unbind_shader();
}

void font_renderer_cleanup(void)
{
    destroy_font_shader(gp_font_shader);
    FT_Done_Library(gp_ft);
}

