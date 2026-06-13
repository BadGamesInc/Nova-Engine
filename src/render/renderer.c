//
// Created by gregorym on 5/10/26.
//

#include <stdio.h>
#include <gl.h>
#include <cglm.h>
#include "render/renderer.h"
#include "render/shader.h"
#include "render/model/model.h"
#include "render/model/resource_loader.h"
#include "render/asset_manager.h"
#include "render/entity_renderer.h"
#include "util/mathutils.h"
#include "util/containers.h"
#include "engine.h"
#include "render/terrain_renderer.h"

// Global variables
//
static HashMap *       gp_batched_entities = NULL;
static ArrayList *     gp_terrains         = NULL;
static EntityShader *  gp_entity_shader    = NULL;
static TerrainShader * gp_terrain_shader   = NULL;
static mat4            g_projection_mat    = { { 0.0f } };

void renderer_init (void)
{
    init_resource_loader();

    // Initialize globals
    //
    gp_batched_entities       = create_hashmap(
        16, int_hash, container_int_compare, container_int_compare);
    gp_terrains       = create_arraylist(16, STRUCT_COMPARE_FUNC(Terrain, id));
    gp_entity_shader  = create_entity_shader();
    gp_terrain_shader = create_terrain_shader();

    // Create projection matrix
    //
    glm_perspective(
        FOV, get_aspect_ratio(), NEAR_PLANE, FAR_PLANE, g_projection_mat);

    // Initialize sub-render systems
    //
    init_entity_renderer(gp_entity_shader, g_projection_mat);
    init_terrain_renderer(gp_terrain_shader, g_projection_mat);
}

void enable_culling (void)
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void disable_culling (void)
{
    glDisable(GL_CULL_FACE);
}

// Prepare the frame for rendering
//
void prepare_frame (void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(SKY_COLOR[0], SKY_COLOR[1], SKY_COLOR[2], 0.0f);
    glEnable(GL_DEPTH_TEST);
    enable_culling();
}

// Unbind currently bound model and it's VAO
//
void unbind_model (void)
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindVertexArray(0);
    enable_culling();
}

/**
 * Processes an Entity to be batch rendered
 * @param entity The entity to process for rendering
 */
void process_entity (Entity * entity)
{
    unsigned int * p_model_id = entity->p_model->asset_id;
    ArrayList *    batch      = hashmap_get(gp_batched_entities, p_model_id);

    if (batch != NULL)
    {
        arraylist_add(batch, entity);
    }
    else
    {
        ArrayList * new_batch
            = create_arraylist(16, STRUCT_COMPARE_FUNC(Entity, id));
        arraylist_add(new_batch, entity);
        hashmap_put(gp_batched_entities, p_model_id, new_batch);
    }
}

void free_entity_list_loop (void * p_value)
{
    arraylist_free((ArrayList *)p_value, NULL);
}

void process_terrain (Terrain * p_terrain)
{
    arraylist_add(gp_terrains, p_terrain);
}

/**
 * Renders the main scene, called in the main engine render method
 * @param p_camera The camera to view the scene from
 * @param p_light The main light to illuminate the scene
 */
void renderer_render_main (Camera * p_camera, const Light * p_light)
{
    prepare_frame();

    // Set the view matrix
    mat4 view = { { 0 } };
    get_view_matrix(p_camera, view);

    // Entity rendering
    //
    bind_shader((Shader *)gp_entity_shader);
    entity_shader_load_light(gp_entity_shader, p_light);
    entity_shader_load_fog_values(gp_entity_shader, FOG_DENSITY, FOG_GRADIENT, SKY_COLOR);
    shader_uniform_mat4((Shader *)gp_entity_shader, "view", view);
    batch_render_entity(gp_batched_entities);
    unbind_shader();


    // Terrain rendering
    //
    bind_shader((Shader *)gp_terrain_shader);
    terrain_shader_load_light(gp_terrain_shader, p_light);
    terrain_shader_load_fog_values(gp_terrain_shader, FOG_DENSITY, FOG_GRADIENT, SKY_COLOR);
    shader_uniform_mat4((Shader *)gp_terrain_shader, "view", view);
    render_terrains(gp_terrains);
    unbind_shader();

    // Clear the containers to repopulate on the next frame
    hashmap_clear(gp_batched_entities, NULL, free_entity_list_loop);
    arraylist_clear(gp_terrains, NULL);
}

void free_entity_list_clean (void * p_value)
{
    arraylist_free((ArrayList *)p_value, destroy_entity);
}

void renderer_cleanup (void)
{
    clean_resources();
    free_hashmap(gp_batched_entities, free, free_entity_list_clean);
    destroy_entity_shader(gp_entity_shader);
    destroy_terrain_shader(gp_terrain_shader);
}