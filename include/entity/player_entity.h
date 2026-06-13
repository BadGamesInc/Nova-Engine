//
// Created by gregorym on 6/10/26.
//

#ifndef NOVA_ENGINE_PLAYER_ENTITY_H
#define NOVA_ENGINE_PLAYER_ENTITY_H
#include "entity.h"

typedef struct
{
    Entity entity;
} PlayerEntity;

void create_player_entity(PlayerEntity * p_player);

#endif //NOVA_ENGINE_PLAYER_ENTITY_H
