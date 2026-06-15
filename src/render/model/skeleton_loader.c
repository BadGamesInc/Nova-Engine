//
// Created by gregorym on 6/13/26.
//

#include "render/model/skeleton_loader.h"
#include <string.h>
#include <stdlib.h>

#include <assimp/scene.h>
#include <assimp/anim.h>
#include <cglm.h>

#include "util/nova_logger.h"

static void ai_to_glm (const struct aiMatrix4x4 * src, mat4 dest)
{
    dest[0][0] = src->a1;
    dest[1][0] = src->a2;
    dest[2][0] = src->a3;
    dest[3][0] = src->a4;
    dest[0][1] = src->b1;
    dest[1][1] = src->b2;
    dest[2][1] = src->b3;
    dest[3][1] = src->b4;
    dest[0][2] = src->c1;
    dest[1][2] = src->c2;
    dest[2][2] = src->c3;
    dest[3][2] = src->c4;
    dest[0][3] = src->d1;
    dest[1][3] = src->d2;
    dest[2][3] = src->d3;
    dest[3][3] = src->d4;
}

static int find_or_add_bone (const Skeleton * p_skeleton,
                             const char *     name,
                             int *            p_bone_count)
{
    for (int i = 0; i < *p_bone_count; i++)
    {
        if (strcmp(p_skeleton->p_bones[i].name, name) == 0)
        {
            return i;
        }
    }

    if (*p_bone_count >= MAX_BONES)
    {
        nova_error("Exceeded MAX_BONES (%d)", MAX_BONES);
        return -1;
    }

    const int index = *p_bone_count;
    strncpy(p_skeleton->p_bones[index].name, name, 63);
    p_skeleton->p_bones[index].parent_index = -1;
    glm_mat4_identity(p_skeleton->p_bones[index].offset_matrix);
    (*p_bone_count)++;
    return index;
}

static void build_hierarchy (Skeleton *            p_skeleton,
                             const struct aiNode * p_node,
                             const int             parent_index,
                             int *                 p_bone_count)
{
    const char * name  = p_node->mName.data;
    int          index = -1;

    // Only register nodes that correspond to actual bones
    for (int i = 0; i < *p_bone_count; i++)
    {
        if (strcmp(p_skeleton->p_bones[i].name, name) == 0)
        {
            index                               = i;
            p_skeleton->p_bones[i].parent_index = parent_index;
            break;
        }
    }

    for (unsigned int i = 0; i < p_node->mNumChildren; i++)
    {
        build_hierarchy(p_skeleton,
                        p_node->mChildren[i],
                        index >= 0 ? index : parent_index,
                        p_bone_count);
    }
}

Skeleton * load_skeleton_from_scene (const struct aiScene * p_scene)
{
    // Count unique bones across all meshes
    int total_bones = 0;
    for (unsigned int m = 0; m < p_scene->mNumMeshes; m++)
    {
        total_bones += (int)p_scene->mMeshes[m]->mNumBones;
    }

    if (total_bones == 0)
    {
        return NULL; // static mesh, no skeleton needed
    }

    Skeleton * p_skeleton
        = create_skeleton(MAX_BONES, (int)p_scene->mNumAnimations);
    if (p_skeleton == NULL)
    {
        return NULL;
    }

    int bone_count = 0;

    // --- Pass 1: register all bones and store offset matrices ---
    for (unsigned int m = 0; m < p_scene->mNumMeshes; m++)
    {
        const struct aiMesh * mesh = p_scene->mMeshes[m];
        for (unsigned int b = 0; b < mesh->mNumBones; b++)
        {
            const struct aiBone * ai_bone = mesh->mBones[b];
            int                   idx     = find_or_add_bone(
                p_skeleton, ai_bone->mName.data, &bone_count);
            if (idx >= 0)
            {
                ai_to_glm(&ai_bone->mOffsetMatrix,
                          p_skeleton->p_bones[idx].offset_matrix);
            }
        }
    }

    p_skeleton->bone_count = bone_count;

    build_hierarchy(p_skeleton, p_scene->mRootNode, -1, &bone_count);

    mat4 root_transform;
    ai_to_glm(&p_scene->mRootNode->mTransformation, root_transform);
    glm_mat4_inv(root_transform, p_skeleton->global_inverse_transform);

    for (unsigned int a = 0; a < p_scene->mNumAnimations; a++)
    {
        const struct aiAnimation * ai_anim = p_scene->mAnimations[a];
        Animation *                anim    = &p_skeleton->p_animations[a];

        strncpy(anim->name, ai_anim->mName.data, 63);
        anim->duration = ai_anim->mDuration;
        anim->ticks_per_second
            = ai_anim->mTicksPerSecond > 0.0 ? ai_anim->mTicksPerSecond : 24.0;
        anim->channel_count = (int)ai_anim->mNumChannels;
        anim->p_channels    = calloc(anim->channel_count, sizeof(BoneChannel));

        if (anim->p_channels == NULL)
        {
            nova_error("Failed to allocate channels for animation %s",
                       anim->name);
            continue;
        }

        for (unsigned int c = 0; c < ai_anim->mNumChannels; c++)
        {
            const struct aiNodeAnim * ai_ch = ai_anim->mChannels[c];
            BoneChannel *             ch    = &anim->p_channels[c];

            strncpy(ch->bone_name, ai_ch->mNodeName.data, 63);

            // Resolve bone index
            ch->bone_index = -1;
            for (int i = 0; i < p_skeleton->bone_count; i++)
            {
                if (strcmp(p_skeleton->p_bones[i].name, ch->bone_name) == 0)
                {
                    ch->bone_index = i;
                    break;
                }
            }

            // Position keys
            ch->position_count = (int)ai_ch->mNumPositionKeys;
            ch->p_positions = malloc(sizeof(PositionKey) * ch->position_count);
            for (int k = 0; k < ch->position_count; k++)
            {
                ch->p_positions[k].time     = ai_ch->mPositionKeys[k].mTime;
                ch->p_positions[k].value[0] = ai_ch->mPositionKeys[k].mValue.x;
                ch->p_positions[k].value[1] = ai_ch->mPositionKeys[k].mValue.y;
                ch->p_positions[k].value[2] = ai_ch->mPositionKeys[k].mValue.z;
            }

            // Rotation keys (aiQuaternion → cglm versors: w,x,y,z)
            ch->rotation_count = (int)ai_ch->mNumRotationKeys;
            ch->p_rotations = malloc(sizeof(RotationKey) * ch->rotation_count);
            for (int k = 0; k < ch->rotation_count; k++)
            {
                ch->p_rotations[k].time     = ai_ch->mRotationKeys[k].mTime;
                ch->p_rotations[k].value[0] = ai_ch->mRotationKeys[k].mValue.x;
                ch->p_rotations[k].value[1] = ai_ch->mRotationKeys[k].mValue.y;
                ch->p_rotations[k].value[2] = ai_ch->mRotationKeys[k].mValue.z;
                ch->p_rotations[k].value[3] = ai_ch->mRotationKeys[k].mValue.w;
            }

            // Scale keys
            ch->scale_count = (int)ai_ch->mNumScalingKeys;
            ch->p_scales    = malloc(sizeof(ScaleKey) * ch->scale_count);
            for (int k = 0; k < ch->scale_count; k++)
            {
                ch->p_scales[k].time     = ai_ch->mScalingKeys[k].mTime;
                ch->p_scales[k].value[0] = ai_ch->mScalingKeys[k].mValue.x;
                ch->p_scales[k].value[1] = ai_ch->mScalingKeys[k].mValue.y;
                ch->p_scales[k].value[2] = ai_ch->mScalingKeys[k].mValue.z;
            }
        }

        nova_info("Loaded animation '%s': %.0f ticks @ %.0f tps, %d channels",
                  anim->name,
                  anim->duration,
                  anim->ticks_per_second,
                  anim->channel_count);
    }

    nova_info("Skeleton loaded: %d bones, %d animations",
              p_skeleton->bone_count,
              p_skeleton->animation_count);
    return p_skeleton;
}

void extract_bone_weights (const struct aiMesh * p_mesh,
                           const Skeleton *      p_skeleton,
                           int *                 bone_indices,
                           float *               bone_weights)
{
    const int vertex_count = (int)p_mesh->mNumVertices;

    // Initialize: -1 index means "no bone"
    for (int i = 0; i < vertex_count * MAX_BONE_INFLUENCE; i++)
    {
        bone_indices[i] = -1;
        bone_weights[i] = 0.0f;
    }

    for (unsigned int b = 0; b < p_mesh->mNumBones; b++)
    {
        const struct aiBone * ai_bone = p_mesh->mBones[b];

        // Find bone index in our skeleton
        int bone_idx = -1;
        for (int i = 0; i < p_skeleton->bone_count; i++)
        {
            if (strcmp(p_skeleton->p_bones[i].name, ai_bone->mName.data) == 0)
            {
                bone_idx = i;
                break;
            }
        }
        if (bone_idx < 0)
        {
            continue;
        }

        for (unsigned int w = 0; w < ai_bone->mNumWeights; w++)
        {
            const int   vertex_id = (int)ai_bone->mWeights[w].mVertexId;
            const float weight    = ai_bone->mWeights[w].mWeight;

            // Find a free slot in this vertex's influence list
            for (int slot = 0; slot < MAX_BONE_INFLUENCE; slot++)
            {
                const int base = vertex_id * MAX_BONE_INFLUENCE + slot;
                if (bone_indices[base] < 0)
                {
                    bone_indices[base] = bone_idx;
                    bone_weights[base] = weight;
                    break;
                }
            }
        }
    }
}