//
// Created by gregorym on 5/10/26.
//

#ifndef NOVA_ENGINE_MODEL_H
#define NOVA_ENGINE_MODEL_H

#include <stdbool.h>
#include <render/texture.h>

typedef struct
{
    unsigned int vao_id;
    int          vertex_count;
} RawModel;

typedef struct
{
    Texture      texture;
    unsigned int texture_id;
    float        shine_damper;
    float        reflectivity;
    bool         has_transparency;
    bool         use_fake_normals;
} ModelTexture;

typedef struct
{
    RawModel       raw_model;
    ModelTexture   texture;
    unsigned int * asset_id;
} TexturedModel;

RawModel *      create_raw_model(unsigned int vao_id, int vertex_count);
void            destroy_raw_model(RawModel * p_model);
unsigned int    get_vao_id(const RawModel * p_model);
int             get_vertex_count(const RawModel * p_model);
ModelTexture *  create_model_texture(Texture * p_texture);
void            destroy_model_texture(ModelTexture * p_model_texture);
TexturedModel * create_textured_model(RawModel *           p_model,
                                      const ModelTexture * p_texture,
                                      unsigned int         asset_id);
void            destroy_textured_model(TexturedModel * p_textured_model);

#endif // NOVA_ENGINE_MODEL_H
