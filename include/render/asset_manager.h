//
// Created by gregorym on 6/1/26.
//

#ifndef NOVA_ENGINE_ASSET_MANAGER_H
#define NOVA_ENGINE_ASSET_MANAGER_H

#include "model/resource_loader.h"
#include "render/model/model.h"

// Textures
//

// Models
//
extern LoadedModel * gp_dragon;
extern LoadedModel * gp_grass_fan;
extern LoadedModel * gp_fern;
extern LoadedModel * gp_low_poly_tree;
extern LoadedModel * gp_women;

void          init_asset_manager(void);
void          load_textures(void);
void          load_models(void);
void          clean_textures(void);
void          clean_models(void);
void          clean_assets(void);
LoadedModel * get_model_by_id(unsigned int id);

#endif // NOVA_ENGINE_ASSET_MANAGER_H
