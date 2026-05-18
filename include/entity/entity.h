//
// Created by gregorym on 5/17/26.
//

#ifndef NOVA_ENGINE_ENTITY_H
#define NOVA_ENGINE_ENTITY_H

#include <render/model/model.h>
#include <cglm.h>

typedef struct
{
    vec3 rotation;
    vec3 position;
    vec3 scale;
    TexturedModel * p_model;
} Entity;

Entity * create_entity(TexturedModel * p_model, vec3 rotation, vec3 scale, vec3 position);
void     get_position(Entity * p_entity, vec3 dest);
void     set_position(Entity * p_entity, vec3 position);
void     get_scale(Entity * p_entity, vec3 dest);
void     set_scale(Entity * p_entity, vec3 scale);
void     get_rotation(Entity * p_entity, vec3 dest);
void     set_rotation(Entity * p_entity, vec3 rotation);
void     increase_position(Entity * p_entity, vec3 increase);
void     increase_rotation(Entity * p_entity, vec3 increase);

#endif //NOVA_ENGINE_ENTITY_H
