//
// Created by gregorym on 6/13/26.
//

#ifndef NOVA_ENGINE_SKELETON_LOADER_H
#define NOVA_ENGINE_SKELETON_LOADER_H

#include <assimp/scene.h>
#include "render/model/skeleton.h"

// Load the full skeleton and all animations from an aiScene.
// Returns NULL if the scene has no bones (static mesh).
Skeleton * load_skeleton_from_scene(const struct aiScene * p_scene);

// Fill bone index + weight arrays for one mesh, ready for GPU upload.
// bone_indices and bone_weights must be pre-allocated:
//   size = vertex_count * MAX_BONE_INFLUENCE
void extract_bone_weights(const struct aiMesh * p_mesh,
                          const Skeleton *      p_skeleton,
                          int *                 bone_indices,
                          float *               bone_weights);

#endif // NOVA_ENGINE_SKELETON_LOADER_H
