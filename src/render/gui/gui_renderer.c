//
// Created by gregorym on 6/14/26.
//

#include "render/gui/gui_renderer.h"

#include "engine.h"
#include "render/shader.h"
#include "render/gui/tesselator.h"
#include "util/mathutils.h"

Shader *   gp_gui_shader       = NULL;
mat4       g_projection_matrix = {0};
mat4       g_transform_matrix  = {0};
static int g_width             = 0;
static int g_height            = 0;

void gui_renderer_init(void)
{
    tesselator_init();
    gp_gui_shader = create_shader("gui.vert", "gui.frag");
    g_width = get_window_width();
    g_height = get_window_height();
    glm_ortho(0.0f, (float) g_width, (float) g_height, 0.0f, -1.0f, 1.0f, g_projection_matrix);
    bind_shader(gp_gui_shader);
    shader_uniform_mat4(gp_gui_shader, "projection_matrix", g_projection_matrix);
    unbind_shader();
    glm_mat4_identity(g_transform_matrix);
    font_renderer_init(g_projection_matrix);
}

static void prepare(void)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

static void finish(void)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

void gui_renderer_render(void)
{
    prepare();
    bind_shader(gp_gui_shader);
    shader_uniform_mat4(gp_gui_shader, "projection_matrix", g_projection_matrix);
    shader_uniform_mat4(gp_gui_shader, "transformation_matrix", g_transform_matrix);
    shader_uniform_bool(gp_gui_shader, "use_texture", false);
    shader_uniform_bool(gp_gui_shader, "use_color", true);

    gui_renderer_draw_rect(10, 10, 100, 100, (vec4) { 1.0f, 0.0f, 0.0f, 1.0f });
    gui_renderer_draw_string(gp_default_font, "Hello World!", 10, 10, 3.0f, (vec3) { 1.0f, 1.0f, 1.0f });

    unbind_shader();
    finish();
}

void gui_renderer_draw_string(const Font * p_font, const char * p_text, const float x, const float y, const float scale, vec3 color)
{
    unbind_shader();
    font_renderer_draw_string(p_font, p_text, x, y, scale, color);
    bind_shader(gp_gui_shader);
}

void gui_renderer_draw_rect(float x, float y, float width, float height, vec4 color)
{
    tesselator_begin_formatted(GL_TRIANGLES, true, false);
    tesselator_fcolor(color[0], color[1], color[2], color[3]);

    tesselator_vertex(x, y);
    tesselator_vertex(x + width, y);
    tesselator_vertex(x + width, y + height);

    tesselator_vertex(x, y);
    tesselator_vertex(x + width, y + height);
    tesselator_vertex(x, y + height);

    tesselator_end();
}

void gui_renderer_draw_rect_outline(float x, float y, float width, float height, float line_width, vec4 color)
{
    float prev_line_width = 0;
    glGetFloatv(GL_LINE_WIDTH, &prev_line_width);

    tesselator_begin_formatted(GL_LINES, true, false);
    tesselator_fcolor(color[0], color[1], color[2], color[3]);

    tesselator_vertex(x, y);
    tesselator_vertex(x + width, y);
    tesselator_vertex(x + width, y + height);
    tesselator_vertex(x, y + height);

    tesselator_end();
    glLineWidth(prev_line_width);
}

void gui_renderer_draw_gradient_rect(float x, float y, float width, float height, vec4 color1, vec4 color2)
{
    tesselator_begin_formatted(GL_TRIANGLES, true, false);

    tesselator_fcolor(color1[0], color1[1], color1[2], color1[3]);
    tesselator_vertex(x, y);
    tesselator_vertex(x + width, y);
    tesselator_fcolor(color2[0], color2[1], color2[2], color2[3]);
    tesselator_vertex(x + width, y + height);

    tesselator_fcolor(color1[0], color1[1], color1[2], color1[3]);
    tesselator_vertex(x, y);
    tesselator_fcolor(color2[0], color2[1], color2[2], color2[3]);
    tesselator_vertex(x + width, y + height);
    tesselator_vertex(x, y + height);

    tesselator_end();
}

void gui_renderer_draw_textured_rect_uv(float x, float y, float width, float height, float u0, float v0, float u1, float v1)
{
    shader_uniform_bool(gp_gui_shader, "use_texture", true);
    shader_uniform_bool(gp_gui_shader, "use_color", false);
    tesselator_begin_formatted(GL_TRIANGLES, false, true);

    tesselator_vertex_texture(x, y, u0, v0);
    tesselator_vertex_texture(x + width, y, u1, v0);
    tesselator_vertex_texture(x + width, y + height, u1, v1);

    tesselator_vertex_texture(x, y, u0, v0);
    tesselator_vertex_texture(x + width, y + height, u1, v1);
    tesselator_vertex_texture(x, y + height, u0, v1);

    tesselator_end();
    shader_uniform_bool(gp_gui_shader, "use_texture", false);
    shader_uniform_bool(gp_gui_shader, "use_color", true);
}

void gui_renderer_draw_textured_rect(float x, float y, float width, float height)
{
    gui_renderer_draw_textured_rect_uv(x, y, width, height, 0.0f, 0.0f, 1.0f, 1.0f);
}

void gui_renderer_draw_textured_rect_colored(float x, float y, float width, float height, float u0, float v0, float u1, float v1, vec4 color)
{
    tesselator_begin_formatted(GL_TRIANGLES, true, true);
    tesselator_fcolor(color[0], color[1], color[2], color[3]);

    tesselator_vertex_texture(x, y, u0, v0);
    tesselator_vertex_texture(x + width, y, u1, v0);
    tesselator_vertex_texture(x + width, y + height, u1, v1);

    tesselator_vertex_texture(x, y, u0, v0);
    tesselator_vertex_texture(x + width, y + height, u1, v1);
    tesselator_vertex_texture(x, y + height, u0, v1);

    tesselator_end();
}

void gui_renderer_draw_line(float x1, float y1, float x2, float y2, float line_width, vec4 color)
{
    float prev_line_width = 0;
    glGetFloatv(GL_LINE_WIDTH, &prev_line_width);

    tesselator_begin_formatted(GL_LINES, true, false);
    tesselator_fcolor(color[0], color[1], color[2], color[3]);

    tesselator_vertex(x1, y1);
    tesselator_vertex(x2, y2);

    tesselator_end();
    glLineWidth(prev_line_width);
}

void gui_renderer_draw_circle(float cx, float cy, float radius, int segments, vec4 color)
{
    tesselator_begin_formatted(GL_TRIANGLE_FAN, true, false);
    tesselator_fcolor(color[0], color[1], color[2], color[3]);

    tesselator_vertex(cx, cy);
    for (int i = 0; i < segments; i++)
    {
        const float angle = (float) (2.0 * PI * i / segments);
        const float x = cx + cosf(angle) * radius;
        const float y = cy + sinf(angle) * radius;
        tesselator_vertex(x, y);
    }

    tesselator_end();
}

void gui_renderer_resize(int width, int height)
{
    g_width = width;
    g_height = height;
    glm_ortho(0.0f, (float) g_width, (float) g_height, 0.0f, -1.0f, 1.0f, g_projection_matrix);
    font_renderer_resize(g_projection_matrix);
}

void gui_renderer_cleanup(void)
{
    destroy_shader(gp_gui_shader);
    tesselator_cleanup();
}
