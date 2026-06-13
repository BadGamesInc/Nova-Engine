//
// Created by gregorym on 5/10/26.
//

#include <render/model/model.h>
#include <util/nova_logger.h>
#include <gl.h>
#include <stdlib.h>
#include <stdint.h>

/**
 * Creates a raw model
 * @param vao_id The VAO to use
 * @param vertex_count The amount of vertices the model has
 * @return The pointer to the newly created RawModel
 */
RawModel * create_raw_model (const unsigned int vao_id, const int vertex_count)
{
    RawModel * p_model = malloc(sizeof(*p_model));

    if (!p_model)
    {
        nova_error("Failed to create RawModel pointer");
        return NULL;
    }

    p_model->vao_id       = vao_id;
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
ModelTexture * create_model_texture (Texture * p_texture)
{
    ModelTexture * p_model_texture = malloc(sizeof(*p_model_texture));

    if (p_model_texture == NULL)
    {
        nova_error("Failed to create ModelTexture");
        return NULL;
    }

    p_model_texture->texture          = *p_texture;
    p_model_texture->texture_id       = p_texture->texture_id;
    p_model_texture->shine_damper     = 1.0f;
    p_model_texture->reflectivity     = 0.0f;
    p_model_texture->has_transparency = false;
    p_model_texture->use_fake_normals = false;

    free(p_texture);

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
 * @param asset_id The unique number to identify this model
 * @return The pointer to the newly created TexturedModel
 */
TexturedModel * create_textured_model (RawModel *           p_model,
                                       const ModelTexture * p_texture,
                                       const unsigned int   asset_id)
{
    TexturedModel * p_textured_model = malloc(sizeof(*p_textured_model));

    if (p_textured_model == NULL)
    {
        nova_error("Failed to create TexturedModel");
        return NULL;
    }

    p_textured_model->raw_model = *p_model;
    p_textured_model->texture   = *p_texture;
    p_textured_model->asset_id  = malloc(sizeof(unsigned int));
    *p_textured_model->asset_id = asset_id;

    free(p_model);

    return p_textured_model;
}

// Free TextureModel from memory
//
void destroy_textured_model (TexturedModel * p_textured_model)
{
    free(p_textured_model);
}