//
// Created by gregorym on 5/30/26.
//

#include <gl.h>
#include <cglm.h>
#include "render/model/model.h"
#include "render/shader.h"
#include "render/renderer.h"
#include "entity/entity.h"
#include "render/entity_renderer.h"

#include "render/asset_manager.h"
#include "render/model/resource_loader.h"
#include "util/mathutils.h"
#include "util/nova_logger.h"

static EntityShader * gp_entity_shader = NULL;

/**
 * Initializes the entity renderer
 */
void init_entity_renderer (EntityShader * p_shader, mat4 projection_matrix)
{
    gp_entity_shader = p_shader;

    // Load matrices to shader
    //
    bind_shader((Shader *)gp_entity_shader);
    shader_uniform_mat4(
        (Shader *)gp_entity_shader, "projection", projection_matrix);
    unbind_shader();
}

// Prepare the entity transformation
//
static void prepare_entity (Entity * p_entity, const MeshEntry * p_mesh)
{
    mat4 trans = { { 0 } };
    create_transformation_matrix(
        trans, p_entity->position, p_entity->rotation, p_entity->scale);

    // Load values to shader
    shader_uniform_mat4((Shader *)gp_entity_shader, "transformation", trans);
    entity_shader_load_tex_offset(gp_entity_shader,
                                  entity_get_tex_x_offset(p_entity, p_mesh),
                                  entity_get_tex_y_offset(p_entity, p_mesh));
}

static void upload_bone_matrices (const AnimationState * p_state)
{
    for (int i = 0; i < p_state->p_skeleton->bone_count; i++)
    {
        char uniform_name[32];
        snprintf(uniform_name, sizeof(uniform_name), "bone_matrices[%d]", i);
        shader_uniform_mat4((Shader *)gp_entity_shader,
                            uniform_name,
                            p_state->bone_matrices[i]);
    }
}

static void prepare_mesh_entry (const MeshEntry * p_mesh)
{
    glBindVertexArray(p_mesh->p_raw_model->vao_id);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    if (p_mesh->is_animated)
    {
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
    }

    if (p_mesh->has_transparency)
    {
        disable_culling();
    }

    shader_uniform_bool((Shader *)gp_entity_shader,
                        "use_fake_normals",
                        p_mesh->use_fake_normals);
    shader_uniform_bool(
        (Shader *)gp_entity_shader, "is_animated", p_mesh->is_animated);
    entity_shader_load_shine_values(
        gp_entity_shader, p_mesh->shine_damper, p_mesh->reflectivity);

    if (p_mesh->p_texture != NULL)
    {
        entity_shader_load_tex_rows(gp_entity_shader,
                                    p_mesh->p_texture->num_rows);
        bind_texture(p_mesh->p_texture, 0);
    }
}

/**
 * Batch renders all the entities within the provided HashMap
 * @param p_entity_map The entities to render
 */
void batch_render_entity (const HashMap * p_entity_map)
{
    const ArrayList * p_keys = hashmap_keys(p_entity_map);

    for (int i = 0; i < arraylist_count(p_keys); i++)
    {
        const unsigned int * p_asset_id = arraylist_get(p_keys, i);
        const ArrayList *    p_batch    = hashmap_get(p_entity_map, p_asset_id);
        const LoadedModel *  p_model    = get_model_by_id(*p_asset_id);

        if (p_model == NULL)
        {
            continue;
        }

        for (int m = 0; m < p_model->mesh_count; m++)
        {
            const MeshEntry * p_mesh = &p_model->p_meshes[m];
            if (p_mesh->p_raw_model == NULL)
            {
                continue;
            }

            prepare_mesh_entry(p_mesh);
            for (int j = 0; j < arraylist_count(p_batch); j++)
            {
                Entity * p_entity = arraylist_get(p_batch, j);
                prepare_entity(p_entity, p_mesh);

                if (p_mesh->is_animated && p_entity->p_animation_state != NULL)
                {
                    upload_bone_matrices(p_entity->p_animation_state);
                }

                glDrawElements(GL_TRIANGLES,
                               p_mesh->p_raw_model->vertex_count,
                               GL_UNSIGNED_INT,
                               0);
            }
            unbind_model();

            if (p_mesh->has_transparency)
            {
                enable_culling();
            }
        }
    }
}

void cleanup_entity_renderer ()
{
    destroy_entity_shader(gp_entity_shader);
}