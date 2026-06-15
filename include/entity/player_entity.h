//
// Created by gregorym on 6/10/26.
//

#ifndef NOVA_ENGINE_PLAYER_ENTITY_H
#define NOVA_ENGINE_PLAYER_ENTITY_H

#include "camera.h"
#include "living_entity.h"
#include "terrain/terrain.h"

typedef struct
{
    LivingEntity entity;
    bool         is_running;
    Animation *  p_current_target_anim;
} PlayerEntity;

PlayerEntity * create_player_entity(LoadedModel * p_model,
                                    vec3          position,
                                    int           texture_index,
                                    unsigned int  id);
void           destroy_player_entity(void * p_player);
void           player_entity_update(PlayerEntity * p_player,
                                    float          delta_time,
                                    const Camera * p_camera);
Terrain *      player_entity_get_current_terrain(const PlayerEntity * p_player);

#endif // NOVA_ENGINE_PLAYER_ENTITY_H
