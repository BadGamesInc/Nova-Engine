//
// Created by gregorym on 5/17/26.
//

#ifndef NOVA_ENGINE_ENTITY_H
#define NOVA_ENGINE_ENTITY_H

#include <cglm.h>

#include "render/model/resource_loader.h"

typedef struct
{
    unsigned int    id;
    vec3            rotation;
    vec3            position;
    vec3            scale;
    LoadedModel *   p_model;
} Entity;

Entity * create_entity(LoadedModel *   p_model,
                       vec3            rotation,
                       vec3            scale,
                       vec3            position,
                       unsigned int    id);
void     destroy_entity(void * p_entity);
void     entity_get_position(Entity * p_entity, vec3 dest);
void     entity_set_position(Entity * p_entity, vec3 position);
void     entity_get_scale(Entity * p_entity, vec3 dest);
void     entity_set_scale(Entity * p_entity, vec3 scale);
void     entity_get_rotation(Entity * p_entity, vec3 dest);
void     entity_set_rotation(Entity * p_entity, vec3 rotation);
void     entity_increase_position(Entity * p_entity, vec3 increase);
void     entity_increase_rotation(Entity * p_entity, vec3 increase);

#endif // NOVA_ENGINE_ENTITY_H
