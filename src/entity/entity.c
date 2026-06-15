//
// Created by gregorym on 5/17/26.
//

#include <entity/entity.h>

#include "util/nova_logger.h"

Entity * create_entity (LoadedModel *      p_model,
                        vec3               rotation,
                        vec3               scale,
                        vec3               position,
                        const int          texture_index,
                        const unsigned int id)
{
    Entity * p_entity = malloc(sizeof(*p_entity));

    if (p_entity == NULL)
    {
        nova_error("Failed to create Entity");
        return NULL;
    }

    glm_vec3_copy(rotation, p_entity->rotation);
    glm_vec3_copy(scale, p_entity->scale);
    glm_vec3_copy(position, p_entity->position);
    p_entity->type          = ENTITY_TYPE_BASE;
    p_entity->p_model       = p_model;
    p_entity->id            = id;
    p_entity->texture_index = texture_index;

    if (p_model->p_skeleton != NULL)
    {
        Animation * idle = skeleton_find_animation(p_model->p_skeleton, "idle");
        p_entity->p_animation_state
            = create_animation_state(p_model->p_skeleton, idle);
    }
    else
    {
        p_entity->p_animation_state = NULL;
    }

    return p_entity;
}

void entity_update (Entity * p_entity, float delta_time)
{
}

float entity_get_tex_x_offset (const Entity *    p_entity,
                               const MeshEntry * p_entry)
{
    const int column = p_entity->texture_index % p_entry->p_texture->num_rows;
    return (float)column / (float)p_entry->p_texture->num_rows;
}

float entity_get_tex_y_offset (const Entity *    p_entity,
                               const MeshEntry * p_entry)
{
    const int row = p_entity->texture_index / p_entry->p_texture->num_rows;
    return (float)row / (float)p_entry->p_texture->num_rows;
}

void destroy_entity (void * p_entity)
{
    free(p_entity);
}

void entity_get_position (Entity * p_entity, vec3 dest)
{
    glm_vec3_copy(p_entity->position, dest);
}

void entity_set_position (Entity * p_entity, vec3 position)
{
    glm_vec3_copy(position, p_entity->position);
}

void entity_get_scale (Entity * p_entity, vec3 dest)
{
    glm_vec3_copy(p_entity->scale, dest);
}

void entity_set_scale (Entity * p_entity, vec3 scale)
{
    glm_vec3_copy(scale, p_entity->scale);
}

void entity_get_rotation (Entity * p_entity, vec3 dest)
{
    glm_vec3_copy(p_entity->rotation, dest);
}

void entity_set_rotation (Entity * p_entity, vec3 rotation)
{
    glm_vec3_copy(rotation, p_entity->rotation);
}

void entity_increase_position (Entity * p_entity, vec3 increase)
{
    glm_vec3_add(p_entity->position, increase, p_entity->position);
}

void entity_increase_rotation (Entity * p_entity, vec3 increase)
{
    glm_vec3_add(p_entity->rotation, increase, p_entity->rotation);
}