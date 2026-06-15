//
// Created by gregorym on 6/12/26.
//
#include "entity/living_entity.h"
#include <cglm.h>

#include "util/nova_logger.h"

LivingEntity * create_living_entity (LoadedModel *      p_model,
                                     const int          texture_index,
                                     vec3               position,
                                     const unsigned int id)
{
    LivingEntity * p_living_entity = malloc(sizeof(*p_living_entity));

    if (p_living_entity == NULL)
    {
        nova_error("Failed to allocate LivingEntity");
        return NULL;
    }

    glm_vec3_copy(GLM_VEC3_ZERO, p_living_entity->entity.rotation);
    glm_vec3_copy(GLM_VEC3_ONE, p_living_entity->entity.scale);
    glm_vec3_copy(position, p_living_entity->entity.position);
    p_living_entity->entity.type          = ENTITY_TYPE_LIVING;
    p_living_entity->entity.p_model       = p_model;
    p_living_entity->entity.id            = id;
    p_living_entity->entity.texture_index = texture_index;
    p_living_entity->yaw                  = 0.0f;
    p_living_entity->pitch                = 0.0f;
    p_living_entity->gravity              = -80.0f;
    p_living_entity->sensitivity          = 0.25f;
    p_living_entity->on_ground            = true;
    p_living_entity->in_air               = false;
    p_living_entity->jump_power           = 40.0f;
    p_living_entity->run_speed            = 25.0f;
    p_living_entity->turn_speed           = 200.0f;
    p_living_entity->up_speed             = 0.0f;
    p_living_entity->current_speed        = 0.0f;
    p_living_entity->current_turn_speed   = 0.0f;
    p_living_entity->is_dead              = false;
    p_living_entity->health               = 100.0f;

    return p_living_entity;
}

void destroy_living_entity (void * p_living_entity)
{
    free(p_living_entity);
}

void living_entity_update (LivingEntity * p_living_entity,
                           const float    delta_time)
{
    entity_update(&p_living_entity->entity, delta_time);
}
