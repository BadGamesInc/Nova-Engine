//
// Created by gregorym on 6/13/26.
//

#ifndef NOVA_ENGINE_SKELETON_H
#define NOVA_ENGINE_SKELETON_H

#include <cglm.h>
#include <stdbool.h>

#define MAX_BONES          100
#define MAX_BONE_INFLUENCE 4

typedef struct
{
    char name[64];
    int  parent_index;
    mat4 offset_matrix;
} Bone;

typedef struct
{
    double time;
    vec3   value;
} PositionKey;

typedef struct
{
    double time;
    versor value;
} RotationKey;

typedef struct
{
    double time;
    vec3   value;
} ScaleKey;

typedef struct
{
    char          bone_name[64];
    int           bone_index;
    PositionKey * p_positions;
    int           position_count;
    RotationKey * p_rotations;
    int           rotation_count;
    ScaleKey *    p_scales;
    int           scale_count;
} BoneChannel;

typedef struct
{
    char          name[64];
    double        duration;
    double        ticks_per_second;
    BoneChannel * p_channels;
    int           channel_count;
} Animation;

typedef struct
{
    Bone *      p_bones;
    int         bone_count;
    Animation * p_animations;
    int         animation_count;
    mat4        global_inverse_transform;
} Skeleton;

typedef struct
{
    Skeleton *  p_skeleton;
    Animation * p_anim_a;
    Animation * p_anim_b;
    double      time_a;
    double      time_b;
    float       blend;
    float       target_blend;
    float       blend_speed;
    mat4        bone_matrices[MAX_BONES];
} AnimationState;

Skeleton *       create_skeleton(int bone_count, int animation_count);
void             destroy_skeleton(Skeleton * p_skeleton);
AnimationState * create_animation_state(Skeleton *  p_skeleton,
                                        Animation * p_anim_a);
void             destroy_animation_state(AnimationState * p_animation_state);
void             animation_state_set_animation(AnimationState * p_state,
                                               Animation *      p_animation);
void             animation_state_blend_to(AnimationState * p_state,
                                          Animation *      p_target,
                                          float            target_blend,
                                          float            blend_speed);
void        animation_state_update(AnimationState * p_state, double delta_time);
Animation * skeleton_find_animation(const Skeleton * p_skeleton,
                                    const char *     p_name);

#endif // NOVA_ENGINE_SKELETON_H
