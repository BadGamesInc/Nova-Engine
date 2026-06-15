//
// Created by gregorym on 6/12/26.
//

#ifndef NOVA_ENGINE_LIVING_ENTITY_H
#define NOVA_ENGINE_LIVING_ENTITY_H

#include "entity.h"

typedef struct
{
    Entity entity;
    float  yaw;
    float  pitch;
    float  gravity;
    float  sensitivity;
    bool   on_ground;
    bool   in_air;
    float  jump_power;
    float  run_speed;
    float  turn_speed;
    float  up_speed;
    float  current_speed;
    float  current_turn_speed;
    bool   is_dead;
    float  health;
} LivingEntity;

LivingEntity * create_living_entity(LoadedModel * p_model,
                                    int           texture_index,
                                    vec3          position,
                                    unsigned int  id);
void           destroy_living_entity(void * p_living_entity);
void living_entity_update(LivingEntity * p_living_entity, float delta_time);
#endif // NOVA_ENGINE_LIVING_ENTITY_H
