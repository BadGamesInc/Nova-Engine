//
// Created by gregorym on 6/14/26.
//

#include <gl.h>
#include "render/gui/tesselator.h"

#include <stdlib.h>
#include <string.h>

#include "util/nova_logger.h"

static float *      gp_buffer         = NULL;
static int          g_buffer_size     = 0;
static int          g_buffer_capacity = 0;
static int          g_vertex_count    = 0;
static unsigned int g_vao_id          = 0;
static unsigned int g_vbo_id          = 0;
static float        g_red             = 1.0f;
static float        g_green           = 1.0f;
static float        g_blue            = 1.0f;
static float        g_alpha           = 1.0f;
static float        g_u               = 0.0f;
static float        g_v               = 0.0f;
static bool         g_is_building     = false;
static bool         g_has_color       = false;
static bool         g_has_texture     = false;
static int          g_draw_mode       = 0;
static int          g_vertex_size     = 0;

// Initialize the tesselator
//
void tesselator_init (void)
{
    g_buffer_capacity = TESSELATOR_DEFAULT_BUFFER_SIZE;
    gp_buffer         = calloc(g_buffer_capacity, sizeof(*gp_buffer));
    glGenVertexArrays(1, &g_vao_id);
    glGenBuffers(1, &g_vbo_id);
    glBindVertexArray(g_vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, g_vbo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static void calculate_vertex_size (void)
{
    g_vertex_size = 2;
    if (g_has_color)
    {
        g_vertex_size += 4;
    }
    if (g_has_texture)
    {
        g_vertex_size += 2;
    }
}

void tesselator_begin (const int mode)
{
    if (g_is_building)
    {
        nova_error("Tesselator already building!");
        return;
    }

    g_is_building = true;
    g_draw_mode   = mode;
    g_vertex_size = 0;
    memset(gp_buffer, 0, sizeof(float) * TESSELATOR_DEFAULT_BUFFER_SIZE);
    g_has_color   = false;
    g_has_texture = false;
}

void tesselator_begin_formatted (const int  mode,
                                 const bool color,
                                 const bool texture)
{
    tesselator_begin(mode);
    g_has_color   = color;
    g_has_texture = texture;
    calculate_vertex_size();
}

void tesselator_fcolor (const float r,
                        const float g,
                        const float b,
                        const float a)
{
    g_red   = r;
    g_green = g;
    g_blue  = b;
    g_alpha = a;
}

void tesselator_color (const int r, const int g, const int b, const int a)
{
    tesselator_fcolor((float)r / 255.0f,
                      (float)g / 255.0f,
                      (float)b / 255.0f,
                      (float)a / 255.0f);
}

void tesselator_tex_coord (const float u, const float v)
{
    g_u           = u;
    g_v           = v;
    g_has_texture = true;
}

static void grow_buffer ()
{
    g_buffer_capacity *= 2;
    float * p_temp_buffer
        = realloc(gp_buffer, sizeof(*gp_buffer) * g_buffer_capacity);
    if (p_temp_buffer == NULL)
    {
        nova_error("Failed to grow tesselator buffer");
        return;
    }
    gp_buffer = p_temp_buffer;
}

void tesselator_vertex (const float x, const float y)
{
    if (!g_is_building)
    {
        nova_error("Tesselator not building!");
        return;
    }

    if (g_buffer_capacity - g_buffer_size < g_vertex_size)
    {
        grow_buffer();
    }

    gp_buffer[g_buffer_size++] = x;
    gp_buffer[g_buffer_size++] = y;

    if (g_has_color)
    {
        gp_buffer[g_buffer_size++] = g_red;
        gp_buffer[g_buffer_size++] = g_green;
        gp_buffer[g_buffer_size++] = g_blue;
        gp_buffer[g_buffer_size++] = g_alpha;
    }

    if (g_has_texture)
    {
        gp_buffer[g_buffer_size++] = g_u;
        gp_buffer[g_buffer_size++] = g_v;
    }

    g_vertex_count++;
}

void tesselator_vertex_texture (const float x,
                                const float y,
                                const float u,
                                const float v)
{
    tesselator_tex_coord(u, v);
    tesselator_vertex(x, y);
}

void tesselator_end (void)
{
    if (!g_is_building)
    {
        nova_error("Tesselator not building!");
        return;
    }

    g_is_building = false;

    if (g_vertex_size == 0)
    {
        return;
    }

    calculate_vertex_size();

    glBindVertexArray(g_vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, g_vbo_id);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)sizeof(float) * g_buffer_size,
                 gp_buffer,
                 GL_STREAM_DRAW);
    int stride = g_vertex_size * sizeof(float);
    int offset = 0;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE, stride, (void *)(uintptr_t)offset);
    offset += 2 * sizeof(float);

    if (g_has_color)
    {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1, 4, GL_FLOAT, GL_FALSE, stride, (void *)(uintptr_t)offset);
        offset += 4 * sizeof(float);
    }

    if (g_has_texture)
    {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2, 2, GL_FLOAT, GL_FALSE, stride, (void *)(uintptr_t)offset);
    }

    glDrawArrays(g_draw_mode, 0, g_vertex_count);

    glDisableVertexAttribArray(0);
    if (g_has_color)
    {
        glDisableVertexAttribArray(1);
    }
    if (g_has_texture)
    {
        glDisableVertexAttribArray(2);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    memset(gp_buffer, 0, sizeof(float) * g_buffer_capacity);
}

void tesselator_cleanup (void)
{
    free(gp_buffer);
    glDeleteBuffers(1, &g_vbo_id);
    glDeleteVertexArrays(1, &g_vao_id);
}
