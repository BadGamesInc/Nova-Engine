//
// Created by gregorym on 6/6/26.
//

#include "terrain/terrain.h"

#include <assert.h>
#include <stdlib.h>

#include "render/model/resource_loader.h"
#include "util/colorutils.h"
#include "util/nova_logger.h"

float get_height(const Image * p_image, const int x, const int z)
{
    if (x < 0 || x >= p_image->width || z < 0 || z >= p_image->height)
    {
        return 0;
    }

    vec4 rgba = {0};
    image_get_rgba(p_image, rgba, x, z);
    float height = (float) get_rgba(rgba);
    height += MAX_PIXEL_COLOR / 2.0f;
    height /= MAX_PIXEL_COLOR / 2.0f;
    height *= MAX_HEIGHT;

    return height;
}

RawModel * generate_terrain_mesh (const Image * p_heightmap)
{
    const int    VERTEX_COUNT = p_heightmap->height;
    int          count = VERTEX_COUNT * VERTEX_COUNT;
    float        vertices[count * 3];
    float        normals[count * 3];
    float        texture_coords[count * 2];
    unsigned int indices[6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1)];
    int          vertex_pointer = 0;

    for (int i = 0; i < VERTEX_COUNT; i++)
    {
        for (int j = 0; j < VERTEX_COUNT; j++)
        {
            vertices[vertex_pointer * 3]
                = (float)j / ((float)VERTEX_COUNT - 1) * SIZE;
            vertices[vertex_pointer * 3 + 1] = get_height(p_heightmap, j, i);
            vertices[vertex_pointer * 3 + 2]
                = (float)i / ((float)VERTEX_COUNT - 1) * SIZE;
            normals[vertex_pointer * 3]     = 0;
            normals[vertex_pointer * 3 + 1] = 1;
            normals[vertex_pointer * 3 + 2] = 0;
            texture_coords[vertex_pointer * 2]
                = (float)j / ((float)VERTEX_COUNT - 1);
            texture_coords[vertex_pointer * 2 + 1]
                = (float)i / ((float)VERTEX_COUNT - 1);
            vertex_pointer++;
        }
    }

    int pointer = 0;
    for (int gz = 0; gz < VERTEX_COUNT - 1; gz++)
    {
        for (int gx = 0; gx < VERTEX_COUNT - 1; gx++)
        {
            const int top_left     = (gz * VERTEX_COUNT) + gx;
            const int top_right    = top_left + 1;
            const int bottom_left  = ((gz + 1) * VERTEX_COUNT) + gx;
            const int bottom_right = bottom_left + 1;
            indices[pointer++] = top_left;
            indices[pointer++] = bottom_left;
            indices[pointer++] = top_right;
            indices[pointer++] = top_right;
            indices[pointer++] = bottom_left;
            indices[pointer++] = bottom_right;
        }
    }

    RawModel * model = load_raw_model(
        vertices,        sizeof vertices        / sizeof vertices[0],
        texture_coords,  sizeof texture_coords  / sizeof texture_coords[0],
        indices,         sizeof indices         / sizeof indices[0],
        normals,         sizeof normals         / sizeof normals[0]
    );
    return model;
}

Terrain * create_terrain (const int grid_x, const int grid_z, const Image * p_heightmap, const TerrainTexturePack * p_texture_pack, const Texture * p_blend_map)
{
    Terrain * terrain = malloc(sizeof(*terrain));

    if (terrain == NULL)
    {
        nova_error("Failed to create terrain");
        return NULL;
    }

    RawModel * p_terrain_mesh = generate_terrain_mesh(p_heightmap);
    terrain->model            = *p_terrain_mesh;
    terrain->texture_pack     = *p_texture_pack;
    terrain->blend_map        = *p_blend_map;
    terrain->x                = (float) grid_x * SIZE;
    terrain->z                = (float) grid_z * SIZE;

    free(p_terrain_mesh);

    return terrain;
}

void destroy_terrain(void * p_terrain)
{
    free(p_terrain);
}

/**
 * Create a texture pack for a terrain
 * @param p_bg_texture The background of the terrain
 * @param p_r_texture The texture associated with the red value on the blend map
 * @param p_g_texture The texture associated with the green value on the blend map
 * @param p_b_texture The texture associated with the blue value on the blend map
 * @return
 */
TerrainTexturePack * create_terrain_texture_pack(Texture * p_bg_texture, Texture * p_r_texture, Texture * p_g_texture, Texture * p_b_texture)
{
    TerrainTexturePack * texture_pack = malloc(sizeof(*texture_pack));

    if (texture_pack == NULL)
    {
        nova_error("Failed to create terrain texture pack");
        return NULL;
    }

    texture_pack->bg_texture = *p_bg_texture;
    texture_pack->r_texture  = *p_r_texture;
    texture_pack->g_texture  = *p_g_texture;
    texture_pack->b_texture  = *p_b_texture;

    free(p_bg_texture);
    free(p_r_texture);
    free(p_g_texture);
    free(p_b_texture);

    return texture_pack;
}

void destroy_terrain_texture_pack(TerrainTexturePack * p_texture_pack)
{
    free(p_texture_pack);
}