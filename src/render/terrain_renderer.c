//
// Created by gregorym on 6/6/26.
//

#include <gl.h>
#include "terrain/terrain.h"
#include "render/terrain_renderer.h"
#include "render/renderer.h"
#include "util/mathutils.h"

static TerrainShader * gp_terrain_shader = NULL;

/**
 * Initializes the terrain renderer
 * @param p_shader The terrain shader to use
 */
void init_terrain_renderer (TerrainShader * p_shader, mat4 projection_matrix)
{
    gp_terrain_shader = p_shader;
    bind_shader((Shader *)p_shader);
    shader_uniform_mat4((Shader *)p_shader, "projection", projection_matrix);
    terrain_shader_connect_texture_units(p_shader);
    unbind_shader();
}

void bind_terrain_textures (const Terrain * p_terrain)
{
    const TerrainTexturePack texture_pack = p_terrain->texture_pack;
    bind_texture(&texture_pack.bg_texture, 0);
    bind_texture(&texture_pack.r_texture, 1);
    bind_texture(&texture_pack.g_texture, 2);
    bind_texture(&texture_pack.b_texture, 3);
    bind_texture(&p_terrain->blend_map, 4);
}

// Prepare the model for rendering
//
void prepare_terrain (const Terrain * p_terrain)
{
    const RawModel raw_model = p_terrain->model;

    // Bind VAO and its elements
    glBindVertexArray(raw_model.vao_id);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    bind_terrain_textures(p_terrain);
    terrain_shader_load_shine_values(gp_terrain_shader, 1.0f, 0.0f);
}

// Prepare the transformation matrix
//
void prepare_model_matrix (const Terrain * p_terrain)
{
    mat4 trans = { { 0 } };
    create_transformation_matrix(trans,
                                 (vec3) { p_terrain->x, 0.0f, p_terrain->z },
                                 (vec3) { 0.0f, 0.0f, 0.0f },
                                 (vec3) { 1.0f, 1.0f, 1.0f });

    // Load transformation matrix to shader
    shader_uniform_mat4((Shader *)gp_terrain_shader, "transformation", trans);
}

/**
 * Render all terrains in the provided list
 * @param p_terrains The list of terrains to render
 */
void render_terrains (const ArrayList * p_terrains)
{
    for (int i = 0; i < arraylist_count(p_terrains); i++)
    {
        Terrain * p_terrain = arraylist_get(p_terrains, i);
        prepare_terrain(p_terrain);
        prepare_model_matrix(p_terrain);
        glDrawElements(
            GL_TRIANGLES, p_terrain->model.vertex_count, GL_UNSIGNED_INT, 0);
        unbind_model();
    }
}