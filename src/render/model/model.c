//
// Created by gregorym on 5/10/26.
//

#include <render/model/model.h>
#include <util/novalogger.h>
#include <gl.h>
#include <stdlib.h>
#include <stdint.h>

/**
 * Creates a raw model
 * @param vao_id The VAO to use
 * @param vertex_count The amount of vertices the model has
 * @return The pointer to the newly created RawModel
 */
RawModel * create_raw_model (const uint32_t vao_id, const int32_t vertex_count)
{
    RawModel * p_model = malloc(sizeof(*p_model));

    if (!p_model)
    {
        nova_error("Failed to create RawModel pointer");
        return NULL;
    }

    p_model->vao_id = vao_id;
    p_model->vertex_count = vertex_count;

    return p_model;
}

// Free the RawModel from memory
//
void destroy_raw_model (RawModel * p_model)
{
    free(p_model);
}

/**
 * Creates a texture for use on models
 * @param p_texture The texture to use
 * @return Pointer to the newly created ModelTexture
 */
ModelTexture * create_model_texture (const Texture * p_texture)
{
    ModelTexture * p_model_texture = malloc(sizeof(*p_model_texture));

    if (p_model_texture == NULL)
    {
        nova_error("Failed to create ModelTexture");
        return NULL;
    }

    p_model_texture->texture_id = p_texture->texture_id;

    return p_model_texture;
}

// Free ModelTexture from memory
//
void destroy_model_texture (ModelTexture * p_model_texture)
{
    free(p_model_texture);
}

/**
 * Create a model with a texture on it
 * @param p_model The model to use
 * @param p_texture The texture to use
 * @return The pointer to the newly created TexturedModel
 */
TexturedModel * create_textured_model (const RawModel * p_model, const ModelTexture * p_texture)
{
    TexturedModel * p_textured_model = malloc(sizeof(*p_textured_model));

    if (p_textured_model == NULL)
    {
        nova_error("Failed to create TexturedModel");
        return NULL;
    }

    p_textured_model->raw_model = *p_model;
    p_textured_model->texture = *p_texture;

    return p_textured_model;
}

// Free TextureModel from memory
//
void destroy_textured_model (TexturedModel * p_textured_model)
{
    free(p_textured_model);
}