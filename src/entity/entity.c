//
// Created by gregorym on 5/17/26.
//

#include <entity/entity.h>

#include "util/novalogger.h"

Entity * create_entity(TexturedModel * p_model, vec3 rotation, vec3 scale, vec3 position)
{
    Entity * entity = malloc(sizeof(*entity));

    if (entity == NULL)
    {
        nova_error("Failed to create Entity");
        return NULL;
    }

    glm_vec3_copy(rotation, entity->rotation);
    glm_vec3_copy(scale, entity->scale);
    glm_vec3_copy(position, entity->position);
    entity->p_model = p_model;

    return entity;
}

void get_position(Entity * p_entity, vec3 dest)
{
    glm_vec3_copy(p_entity->position, dest);
}

void set_position(Entity * p_entity, vec3 position)
{
    glm_vec3_copy(position, p_entity->position);
}

void get_scale(Entity * p_entity, vec3 dest)
{
    glm_vec3_copy(p_entity->scale, dest);
}

void set_scale(Entity * p_entity, vec3 scale)
{
    glm_vec3_copy(scale, p_entity->scale);
}

void get_rotation(Entity * p_entity, vec3 dest)
{
    glm_vec3_copy(p_entity->rotation, dest);
}

void set_rotation(Entity * p_entity, vec3 rotation)
{
    glm_vec3_copy(rotation, p_entity->rotation);
}

void increase_position(Entity * p_entity, vec3 increase)
{
    glm_vec3_add(p_entity->position, increase, p_entity->position);
}

void increase_rotation(Entity * p_entity, vec3 increase)
{
    glm_vec3_add(p_entity->rotation, increase, p_entity->rotation);
}