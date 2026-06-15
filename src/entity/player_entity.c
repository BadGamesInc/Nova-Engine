//
// Created by gregorym on 6/10/26.
//
#include "entity/player_entity.h"

#include "engine.h"
#include "render/asset_manager.h"
#include "terrain/terrain.h"
#include "util/nova_logger.h"
#include "util/input.h"
#include "util/mathutils.h"

/**
 * Create a new PlayerEntity
 * @param p_model The model to use for the player
 * @param position The initial position of the player
 * @param texture_index The index of the texture, use 0 if not using a texture
 * atlas
 * @param id The player's entity id
 * @return A pointer to the new player
 */
PlayerEntity * create_player_entity (LoadedModel *      p_model,
                                     vec3               position,
                                     const int          texture_index,
                                     const unsigned int id)
{
    PlayerEntity * p_player_entity = malloc(sizeof(*p_player_entity));

    if (p_player_entity == NULL)
    {
        nova_error("Failed to allocate PlayerEntity");
        return NULL;
    }

    glm_vec3_copy(GLM_VEC3_ZERO, p_player_entity->entity.entity.rotation);
    glm_vec3_copy(GLM_VEC3_ONE, p_player_entity->entity.entity.scale);
    glm_vec3_copy(position, p_player_entity->entity.entity.position);
    p_player_entity->entity.entity.type          = ENTITY_TYPE_PLAYER;
    p_player_entity->entity.entity.p_model       = p_model;
    p_player_entity->entity.entity.id            = id;
    p_player_entity->entity.entity.texture_index = texture_index;
    p_player_entity->entity.yaw                  = 0.0f;
    p_player_entity->entity.pitch                = 0.0f;
    p_player_entity->entity.gravity              = -80.0f;
    p_player_entity->entity.sensitivity          = 0.25f;
    p_player_entity->entity.on_ground            = true;
    p_player_entity->entity.in_air               = false;
    p_player_entity->entity.jump_power           = 40.0f;
    p_player_entity->entity.run_speed            = 25.0f;
    p_player_entity->entity.turn_speed           = 200.0f;
    p_player_entity->entity.up_speed             = 0.0f;
    p_player_entity->entity.current_speed        = 0.0f;
    p_player_entity->entity.current_turn_speed   = 0.0f;
    p_player_entity->entity.is_dead              = false;
    p_player_entity->entity.health               = 100.0f;
    p_player_entity->is_running                  = false;
    p_player_entity->p_current_target_anim       = NULL;

    return p_player_entity;
}

// Free player pointer
//
void destroy_player_entity (void * p_player)
{
    free(p_player);
}

// Get the correct run speed (left shift = sprinting)
//
static float get_run_speed (PlayerEntity * p_player, const float delta_time)
{
    p_player->is_running = is_key_pressed(GLFW_KEY_LEFT_SHIFT);
    return p_player->entity.run_speed * delta_time
           * (p_player->is_running ? 2.0f : 1.0f);
}

// The speed used for jumping
//
static float get_up_speed (const PlayerEntity * p_player,
                           const float          delta_time)
{
    return p_player->entity.gravity * delta_time;
}

// The speed the player should turn
//
static float get_turn_speed (const PlayerEntity * p_player,
                             const float          delta_time)
{
    return p_player->entity.turn_speed * delta_time;
}

// Cause the player to jump
//
static void jump (PlayerEntity * p_player_entity)
{
    p_player_entity->entity.up_speed  = p_player_entity->entity.jump_power;
    p_player_entity->entity.on_ground = false;
    p_player_entity->entity.in_air    = true;
}

// Used for animation smoothing
static float slow_to_x = 0.0f;
static float slow_to_z = 0.0f;

Terrain * player_entity_get_current_terrain (const PlayerEntity * p_player)
{
    vec2 grid_coords;
    get_grid_coords(grid_coords,
                    p_player->entity.entity.position[0],
                    p_player->entity.entity.position[2]);
    return get_terrain_at((int)grid_coords[0], (int)grid_coords[1]);
}

/**
 * Handle all input and cause the player to move
 * @param p_player The player to move
 * @param delta_time The time between ticks
 * @param p_terrain The terrain the player is walking on, used for heights
 * @param p_camera The camera attached to the player
 */
static void handle_input (PlayerEntity * p_player,
                          const float    delta_time,
                          const Camera * p_camera)
{
    // Parent structs
    LivingEntity * p_living = &p_player->entity;
    Entity *       p_base   = &p_living->entity;

    // Set the player's yaw to the inverted camera yaw
    const float cos_yaw = cosf(glm_rad(-p_camera->yaw));
    const float sin_yaw = sinf(glm_rad(-p_camera->yaw));
    float       move_x  = 0.0f;
    float       move_z  = 0.0f;
    bool        moving  = false;

    // Check movement keys and set move_x and move_z accordingly
    //
    if (is_key_pressed(GLFW_KEY_W))
    {
        move_z -= cos_yaw;
        move_x -= sin_yaw;
        moving = true;
    }
    else if (is_key_pressed(GLFW_KEY_S))
    {
        move_z += cos_yaw;
        move_x += sin_yaw;
        moving = true;
    }

    if (is_key_pressed(GLFW_KEY_A))
    {
        move_z += cosf(glm_rad(-p_camera->yaw - 90.0f));
        move_x += sinf(glm_rad(-p_camera->yaw - 90.0f));
        moving                       = true;
        p_living->current_turn_speed = get_turn_speed(p_player, delta_time);
    }
    else if (is_key_pressed(GLFW_KEY_D))
    {
        move_z += cosf(glm_rad(-p_camera->yaw + 90.0f));
        move_x += sinf(glm_rad(-p_camera->yaw + 90.0f));
        moving                       = true;
        p_living->current_turn_speed = -get_turn_speed(p_player, delta_time);
    }
    else
    {
        const float dist = -p_living->current_turn_speed;
        if (fabsf(dist) >= 0.0001f)
        {
            p_living->current_turn_speed += dist / 15.0f;
        }
        else
        {
            p_living->current_turn_speed = 0.0f;
        }
    }

    // Normalize movement vector
    const float magnitude = sqrtf(move_x * move_x + move_z * move_z);
    if (magnitude > 0.0f)
    {
        move_x /= magnitude;
        move_z /= magnitude;
    }

    // Calculate current run speed
    //
    if (moving)
    {
        p_living->current_speed = get_run_speed(p_player, delta_time);
    }
    else
    {
        // Smooth player movement when no buttons pressed
        const float dist = -p_living->current_speed;
        if (fabsf(dist) >= 0.0001f)
        {
            p_living->current_speed += dist / 15.0f;
        }
        else
        {
            p_living->current_speed = 0.0f;
        }
    }

    // Smooth player movement
    //
    const float dist_x = (move_x * p_living->current_speed) - slow_to_x;
    const float dist_z = (move_z * p_living->current_speed) - slow_to_z;

    if (dist_x != 0.0f)
    {
        slow_to_x += dist_x / 75.0f;
    }
    if (dist_z != 0.0f)
    {
        slow_to_z += dist_z / 75.0f;
    }

    p_base->position[0] += slow_to_x;
    p_base->position[2] += slow_to_z;

    // Player rotation
    //
    const float target_rot_y = -glm_deg(atan2f(slow_to_z, slow_to_x)) + 90.0f;
    float       diff         = target_rot_y - p_base->rotation[1];

    while (diff > 180.0f)
    {
        diff -= 360.0f;
    }
    while (diff < -180.0f)
    {
        diff += 360.0f;
    }

    p_living->yaw += diff / 15.0f;
    p_base->rotation[1] = p_living->yaw;

    // Handle jumping and terrain collision
    //
    if (is_key_pressed(GLFW_KEY_SPACE) && p_living->on_ground)
    {
        jump(p_player);
    }

    p_living->up_speed += p_living->gravity * delta_time;
    p_base->position[1] += p_living->up_speed * delta_time;

    const Terrain * p_terrain = player_entity_get_current_terrain(p_player);
    const float     terrain_height
        = p_terrain == NULL ? 0.0f
                            : terrain_get_height_at(p_terrain,
                                                    p_base->position[0],
                                                    p_base->position[2]);
    if (p_base->position[1] <= terrain_height)
    {
        p_living->up_speed  = 0.0f;
        p_living->on_ground = true;
        p_base->position[1] = terrain_height;
    }
    else
    {
        p_living->on_ground = false;
        p_living->in_air    = true;
    }

    Animation * p_idle = skeleton_find_animation(gp_women->p_skeleton, "idle");
    Animation * p_walk = skeleton_find_animation(gp_women->p_skeleton, "walk");
    Animation * p_run  = skeleton_find_animation(gp_women->p_skeleton, "run");
    Animation * p_jump_run
        = skeleton_find_animation(gp_women->p_skeleton, "jump_run");
    Animation * p_jump_idle
        = skeleton_find_animation(gp_women->p_skeleton, "jump_idle");
    AnimationState * p_anim = p_base->p_animation_state;

    if (p_anim != NULL && p_idle != NULL && p_walk != NULL && p_run != NULL)
    {
        Animation * p_target = p_idle;

        if (p_living->current_speed > 0.0f)
        {
            if (p_player->is_running)
            {
                p_target = p_run;
            }
            else
            {
                p_target = p_walk;
            }
        }

        if (p_target != p_player->p_current_target_anim)
        {
            p_player->p_current_target_anim = p_target;
            p_anim->p_anim_a     = p_anim->p_anim_b != NULL ? p_anim->p_anim_b
                                                            : p_anim->p_anim_a;
            p_anim->time_a       = p_anim->time_b;
            p_anim->p_anim_b     = p_target;
            p_anim->time_b       = 0.0;
            p_anim->blend        = 0.0f;
            p_anim->target_blend = 1.0f;
            p_anim->blend_speed  = 5.0f;
        }
    }
}

/**
 * Update player logic, called every tick
 * @param p_player The player to update
 * @param delta_time The time between ticks
 * @param p_camera The camera to follow the player
 */
void player_entity_update (PlayerEntity * p_player,
                           const float    delta_time,
                           const Camera * p_camera)
{
    living_entity_update(&p_player->entity, delta_time);
    handle_input(p_player, delta_time, p_camera);
}
