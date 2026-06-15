//
// Created by gregorym on 6/1/26.
//

#include "render/asset_manager.h"

#include "render/model/resource_loader.h"
#include "util/containers.h"

ArrayList * gp_textures = NULL;
ArrayList * gp_models   = NULL;

// Textures
//

// Models
//
LoadedModel * gp_dragon        = NULL;
LoadedModel * gp_grass_fan     = NULL;
LoadedModel * gp_fern          = NULL;
LoadedModel * gp_low_poly_tree = NULL;
LoadedModel * gp_women         = NULL;

/**
 * Initializes the asset manager
 */
void init_asset_manager (void)
{
    gp_textures
        = create_arraylist(16, STRUCT_COMPARE_FUNC(ModelTexture, texture_id));
    gp_models
        = create_arraylist(16, STRUCT_COMPARE_FUNC(LoadedModel, asset_id));

    load_textures();
    load_models();
}

/**
 * Loads all the textures used in the engine
 */
void load_textures (void)
{
}

void add_model (LoadedModel * p_model, unsigned int * p_asset_counter)
{
    arraylist_add(gp_models, p_model);
    *p_asset_counter += 1;
}

/**
 * Loads all the models used in the engine
 */
void load_models (void)
{
    unsigned int asset_counter = 0;

    gp_dragon = load_model_from_obj("dragon", "dragon.obj", asset_counter);
    add_model(gp_dragon, &asset_counter);

    gp_grass_fan
        = load_model_from_obj("grass", "grassModel.obj", asset_counter);
    add_model(gp_grass_fan, &asset_counter);

    gp_fern = load_model_from_obj("fern", "fern.obj", asset_counter);
    gp_fern->p_meshes[0].p_texture->num_rows = 2;
    add_model(gp_fern, &asset_counter);

    gp_low_poly_tree
        = load_model_from_obj("lowPolyTree", "lowPolyTree.obj", asset_counter);
    add_model(gp_low_poly_tree, &asset_counter);

    gp_women
        = load_model_from_obj("paladin", "WORKING_FEMALE.fbx", asset_counter);
    add_model(gp_women, &asset_counter);
}

LoadedModel * get_model_by_id (const unsigned int id)
{
    for (int i = 0; i < arraylist_count(gp_models); i++)
    {
        LoadedModel * p_model = arraylist_get(gp_models, i);
        if (*p_model->asset_id == id)
        {
            return p_model;
        }
    }

    return NULL;
}

void free_texture (void * key)
{
    destroy_model_texture(key);
}

void free_model (void * key)
{
    destroy_textured_model(key);
}

void clean_textures (void)
{
    arraylist_free(gp_textures, free_texture);
}

void clean_models (void)
{
    arraylist_free(gp_models, destroy_loaded_model);
}

void clean_assets (void)
{
    clean_textures();
    clean_models();
}