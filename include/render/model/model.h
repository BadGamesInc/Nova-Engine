//
// Created by gregorym on 5/10/26.
//

#ifndef NOVA_ENGINE_MODEL_H
#define NOVA_ENGINE_MODEL_H

#include <stdint.h>
#include <render/texture.h>

typedef struct
{
    uint32_t vao_id;
    int32_t  vertex_count;
} RawModel;

typedef struct
{
    uint32_t texture_id;
} ModelTexture;

typedef struct
{
    RawModel     raw_model;
    ModelTexture texture;
} TexturedModel;

RawModel *      create_raw_model (uint32_t vao_id, int vertex_count);
void            destroy_raw_model (RawModel * p_model);
uint32_t        get_vao_id (const RawModel * p_model);
int             get_vertex_count (const RawModel * p_model);
ModelTexture *  create_model_texture (const Texture * p_texture);
void            destroy_model_texture (ModelTexture * p_model_texture);
TexturedModel * create_textured_model (const RawModel * p_model, const ModelTexture * p_texture);
void            destroy_textured_model (TexturedModel * p_textured_model);

#endif //NOVA_ENGINE_MODEL_H
