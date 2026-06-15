//
// Created by gregorym on 6/13/26.
//

#include "render/model/skeleton.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "util/nova_logger.h"

static int find_position_index (const BoneChannel * p_ch, double time)
{
    for (int i = 0; i < p_ch->position_count - 1; i++)
    {
        if (time < p_ch->p_positions[i + 1].time)
        {
            return i;
        }
    }
    return p_ch->position_count - 2;
}

static int find_rotation_index (const BoneChannel * p_ch, double time)
{
    for (int i = 0; i < p_ch->rotation_count - 1; i++)
    {
        if (time < p_ch->p_rotations[i + 1].time)
        {
            return i;
        }
    }
    return p_ch->rotation_count - 2;
}

static int find_scale_index (const BoneChannel * p_ch, double time)
{
    for (int i = 0; i < p_ch->scale_count - 1; i++)
    {
        if (time < p_ch->p_scales[i + 1].time)
        {
            return i;
        }
    }
    return p_ch->scale_count - 2;
}

static void channel_to_matrix (mat4 dest, const BoneChannel * p_ch, double time)
{
    vec3 position = { 0.0f, 0.0f, 0.0f };
    if (p_ch->position_count == 1)
    {
        glm_vec3_copy(p_ch->p_positions[0].value, position);
    }
    else
    {
        int    i   = find_position_index(p_ch, time);
        double t0  = p_ch->p_positions[i].time;
        double t1  = p_ch->p_positions[i + 1].time;
        float  fac = (float)((time - t0) / (t1 - t0));
        glm_vec3_lerp(p_ch->p_positions[i].value,
                      p_ch->p_positions[i + 1].value,
                      fac,
                      position);
    }

    versor rotation;
    glm_quat_identity(rotation);
    if (p_ch->rotation_count == 1)
    {
        glm_quat_copy(p_ch->p_rotations[0].value, rotation);
    }
    else
    {
        int    i   = find_rotation_index(p_ch, time);
        double t0  = p_ch->p_rotations[i].time;
        double t1  = p_ch->p_rotations[i + 1].time;
        float  fac = (float)((time - t0) / (t1 - t0));
        glm_quat_slerp(p_ch->p_rotations[i].value,
                       p_ch->p_rotations[i + 1].value,
                       fac,
                       rotation);
        glm_quat_normalize(rotation);
    }

    vec3 scale = { 1.0f, 1.0f, 1.0f };
    if (p_ch->scale_count == 1)
    {
        glm_vec3_copy(p_ch->p_scales[0].value, scale);
    }
    else
    {
        int    i   = find_scale_index(p_ch, time);
        double t0  = p_ch->p_scales[i].time;
        double t1  = p_ch->p_scales[i + 1].time;
        float  fac = (float)((time - t0) / (t1 - t0));
        glm_vec3_lerp(
            p_ch->p_scales[i].value, p_ch->p_scales[i + 1].value, fac, scale);
    }

    mat4 T;
    mat4 R;
    mat4 S;
    glm_translate_make(T, position);
    glm_quat_mat4(rotation, R);
    glm_scale_make(S, scale);
    glm_mat4_mul(T, R, dest);
    glm_mat4_mul(dest, S, dest);
}

static void compute_local_transforms (const Skeleton *  p_skeleton,
                                      const Animation * p_anim,
                                      double            time,
                                      mat4 *            p_local_transforms)
{
    for (int i = 0; i < p_skeleton->bone_count; i++)
    {
        glm_mat4_identity(p_local_transforms[i]);
    }

    for (int c = 0; c < p_anim->channel_count; c++)
    {
        const BoneChannel * ch = &p_anim->p_channels[c];
        if (ch->bone_index < 0 || ch->bone_index >= p_skeleton->bone_count)
        {
            continue;
        }
        channel_to_matrix(p_local_transforms[ch->bone_index], ch, time);
    }
}

static void blend_local_transforms (
    mat4 * dest, const mat4 * a, const mat4 * b, float blend, int bone_count)
{
    for (int i = 0; i < bone_count; i++)
    {
        // Decompose both into TRS, lerp/slerp, recompose
        // For simplicity we do a component-wise lerp of the matrices —
        // good enough for small blend angles; replace with TRS slerp
        // if you need perfect quality on large rotational blends.
        for (int row = 0; row < 4; row++)
        {
            for (int col = 0; col < 4; col++)
            {
                dest[i][row][col]
                    = a[i][row][col] * (1.0f - blend) + b[i][row][col] * blend;
            }
        }
    }
}

static void build_bone_matrices (const Skeleton * p_skeleton,
                                 mat4 *           p_local_transforms,
                                 mat4 *           p_bone_matrices)
{
    // global_transforms[i] = world-space transform of bone i
    mat4 global_transforms[MAX_BONES];

    for (int i = 0; i < p_skeleton->bone_count; i++)
    {
        const Bone * bone = &p_skeleton->p_bones[i];

        if (bone->parent_index < 0)
        {
            // Root bone: global = globalInverse * local
            glm_mat4_mul(p_skeleton->global_inverse_transform,
                         p_local_transforms[i],
                         global_transforms[i]);
        }
        else
        {
            glm_mat4_mul(global_transforms[bone->parent_index],
                         p_local_transforms[i],
                         global_transforms[i]);
        }

        // Final matrix = global_transform * offset_matrix
        glm_mat4_mul(global_transforms[i],
                     (vec4 *)bone->offset_matrix,
                     p_bone_matrices[i]);
    }
}

void animation_state_update (AnimationState * p_state, double delta_time)
{
    if (p_state == NULL || p_state->p_anim_a == NULL)
    {
        return;
    }

    float blend_diff = p_state->target_blend - p_state->blend;
    p_state->blend
        += blend_diff * (float)fmin(delta_time * p_state->blend_speed, 1.0f);

    const Skeleton * p_skeleton = p_state->p_skeleton;

    // Advance time, loop both animations
    const double tps_a = p_state->p_anim_a->ticks_per_second > 0.0
                             ? p_state->p_anim_a->ticks_per_second
                             : 24.0;
    p_state->time_a += delta_time * tps_a;
    if (p_state->time_a > p_state->p_anim_a->duration)
    {
        p_state->time_a = fmod(p_state->time_a, p_state->p_anim_a->duration);
    }

    // Local transforms for anim_a
    mat4 local_a[MAX_BONES];
    compute_local_transforms(
        p_skeleton, p_state->p_anim_a, p_state->time_a, local_a);

    mat4 * blended = local_a; // default: no blend

    mat4 local_b[MAX_BONES];
    mat4 blended_storage[MAX_BONES];

    if (p_state->p_anim_b != NULL && p_state->blend > 0.0f)
    {
        const double tps_b = p_state->p_anim_b->ticks_per_second > 0.0
                                 ? p_state->p_anim_b->ticks_per_second
                                 : 24.0;
        p_state->time_b += delta_time * tps_b;
        if (p_state->time_b > p_state->p_anim_b->duration)
        {
            p_state->time_b
                = fmod(p_state->time_b, p_state->p_anim_b->duration);
        }

        compute_local_transforms(
            p_skeleton, p_state->p_anim_b, p_state->time_b, local_b);
        blend_local_transforms(blended_storage,
                               local_a,
                               local_b,
                               p_state->blend,
                               p_skeleton->bone_count);
        blended = blended_storage;
    }

    build_bone_matrices(p_skeleton, blended, p_state->bone_matrices);
}

void animation_state_set_animation (AnimationState * p_state,
                                    Animation *      p_animation)
{
    p_state->p_anim_a = p_animation;
    p_state->p_anim_b = NULL;
    p_state->time_a   = 0.0;
    p_state->time_b   = 0.0;
    p_state->blend    = 0.0f;
}

void animation_state_blend_to (AnimationState * p_state,
                               Animation *      p_target,
                               const float      target_blend,
                               const float      blend_speed)
{
    if (p_state->p_anim_b != p_target)
    {
        p_state->p_anim_b = p_target;
        p_state->time_b   = 0.0;
    }
    p_state->target_blend = target_blend;
    p_state->blend_speed  = blend_speed;
}

Animation * skeleton_find_animation (const Skeleton * p_skeleton,
                                     const char *     p_name)
{
    for (int i = 0; i < p_skeleton->animation_count; i++)
    {
        if (strcmp(p_skeleton->p_animations[i].name, p_name) == 0)
        {
            return &p_skeleton->p_animations[i];
        }
    }
    return NULL;
}

Skeleton * create_skeleton (int bone_count, int animation_count)
{
    Skeleton * sk = malloc(sizeof(Skeleton));
    if (sk == NULL)
    {
        nova_error("Failed to allocate Skeleton");
        return NULL;
    }

    sk->p_bones         = calloc(bone_count, sizeof(Bone));
    sk->p_animations    = calloc(animation_count, sizeof(Animation));
    sk->bone_count      = bone_count;
    sk->animation_count = animation_count;
    glm_mat4_identity(sk->global_inverse_transform);

    return sk;
}

void destroy_skeleton (Skeleton * p_skeleton)
{
    if (p_skeleton == NULL)
    {
        return;
    }
    for (int i = 0; i < p_skeleton->animation_count; i++)
    {
        const Animation * p_anim = &p_skeleton->p_animations[i];
        for (int c = 0; c < p_anim->channel_count; c++)
        {
            free(p_anim->p_channels[c].p_positions);
            free(p_anim->p_channels[c].p_rotations);
            free(p_anim->p_channels[c].p_scales);
        }
        free(p_anim->p_channels);
    }
    free(p_skeleton->p_animations);
    free(p_skeleton->p_bones);
    free(p_skeleton);
}

AnimationState * create_animation_state (Skeleton *  p_skeleton,
                                         Animation * p_anim_a)
{
    AnimationState * state = malloc(sizeof(AnimationState));
    if (state == NULL)
    {
        nova_error("Failed to allocate AnimationState");
        return NULL;
    }

    state->p_skeleton = p_skeleton;
    state->p_anim_a   = p_anim_a;
    state->p_anim_b   = NULL;
    state->time_a     = 0.0;
    state->time_b     = 0.0;
    state->blend      = 0.0f;

    for (int i = 0; i < MAX_BONES; i++)
    {
        glm_mat4_identity(state->bone_matrices[i]);
    }

    return state;
}

void destroy_animation_state (AnimationState * p_animation_state)
{
    free(p_animation_state);
}