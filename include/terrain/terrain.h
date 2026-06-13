//
// Created by gregorym on 6/6/26.
//

#ifndef NOVA_ENGINE_TERRAIN_H
#define NOVA_ENGINE_TERRAIN_H

#define SIZE            800
#define MAX_HEIGHT      40
#define MAX_PIXEL_COLOR (256 * 256 * 256)

#include "render/model/model.h"

typedef struct
{
    Texture bg_texture;
    Texture r_texture;
    Texture g_texture;
    Texture b_texture;
} TerrainTexturePack;

typedef struct
{
    unsigned int       id;
    float              x;
    float              z;
    RawModel           model;
    TerrainTexturePack texture_pack;
    Texture            blend_map;
} Terrain;

TerrainTexturePack * create_terrain_texture_pack(Texture * p_bg_texture, Texture * p_r_texture, Texture * p_g_texture, Texture * p_b_texture);
void                 destroy_terrain_texture_pack(TerrainTexturePack * p_texture_pack);
Terrain *            create_terrain(int grid_x, int grid_z, const Image * p_heightmap, const TerrainTexturePack * p_texture_pack, const Texture * p_blend_map);
void                 destroy_terrain(void * p_terrain);

#endif // NOVA_ENGINE_TERRAIN_H
