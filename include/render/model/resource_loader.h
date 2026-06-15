//
// Created by gregorym on 5/10/26.
//

#ifndef NOVA_ENGINE_RESOURCE_LOADER_H
#define NOVA_ENGINE_RESOURCE_LOADER_H

#include <render/model/model.h>

#include "skeleton.h"
#include "render/texture.h"

typedef struct
{
    RawModel * p_raw_model;
    Texture *  p_texture;
    float      shine_damper;
    float      reflectivity;
    bool       has_transparency;
    bool       use_fake_normals;
    bool       is_animated;
} MeshEntry;

typedef struct
{
    MeshEntry *    p_meshes;
    unsigned int   mesh_count;
    unsigned int * asset_id;
    Skeleton *     p_skeleton;
} LoadedModel;

RawModel *    load_raw_model(const float *        p_vertices,
                             unsigned int         vertices_length,
                             const float *        p_texture_coords,
                             unsigned int         texture_coords_length,
                             const unsigned int * p_indices,
                             unsigned int         indices_length,
                             const float *        p_normals,
                             unsigned int         normals_length,
                             const int *          p_bone_indices,
                             const float *        p_bone_weights,
                             unsigned int         vertex_count);
Texture *     load_texture(const char * texture_name);
void          clean_resources(void);
void          init_resource_loader(void);
RawModel *    load_from_obj_file(const char * p_obj_filename);
LoadedModel * load_model_from_obj(const char * p_model_folder,
                                  const char * p_obj_filename,
                                  unsigned int asset_id);
void          destroy_loaded_model(void * p_value);

#endif // NOVA_ENGINE_RESOURCE_LOADER_H
