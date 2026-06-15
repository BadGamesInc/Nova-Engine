//
// Created by gregorym on 6/6/26.
//

#include "terrain/terrain.h"

#include <assert.h>
#include <stdlib.h>

#include "render/model/resource_loader.h"
#include "util/colorutils.h"
#include "util/mathutils.h"
#include "util/nova_logger.h"

static float get_height (const Image * p_image,
                         const int     x,
                         const int     z,
                         const int     max_height)
{
    if (x < 0 || x >= p_image->width || z < 0 || z >= p_image->height)
    {
        return 0;
    }

    vec4 rgba = { 0 };
    image_get_rgba(p_image, rgba, x, z);
    float height = (float)get_rgba(rgba);
    height += HEIGHTMAP_MAX_PIXEL_COLOR / 2.0f;
    height /= HEIGHTMAP_MAX_PIXEL_COLOR / 2.0f;
    height *= (float)max_height;

    return height;
}

// Calculate the normal for the given coords and store them in dest
//
static void calculate_normal (vec3          dest,
                              const Image * p_image,
                              const int     x,
                              const int     z,
                              const int     max_height)
{
    const float height_l = get_height(p_image, x - 1, z, max_height);
    const float height_r = get_height(p_image, x + 1, z, max_height);
    const float height_d = get_height(p_image, x, z - 1, max_height);
    const float height_u = get_height(p_image, x, z + 1, max_height);
    vec3        normal   = { height_l - height_r, 2.0f, height_d - height_u };
    glm_normalize(normal);

    glm_vec3_copy(normal, dest);
}

static RawModel * generate_terrain_mesh (const Image * p_heightmap,
                                         float **      pp_heights,
                                         const int     max_height)
{
    const int    VERTEX_COUNT = p_heightmap->height;
    int          count        = VERTEX_COUNT * VERTEX_COUNT;
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
                = (float)j / ((float)VERTEX_COUNT - 1) * (float)TERRAIN_SIZE;
            const float height = get_height(p_heightmap, j, i, max_height);
            pp_heights[i][j]   = height;
            vertices[vertex_pointer * 3 + 1] = height;
            vertices[vertex_pointer * 3 + 2]
                = (float)i / ((float)VERTEX_COUNT - 1) * (float)TERRAIN_SIZE;
            vec3 normal = { 0.0f, 0.0f, 0.0f };
            calculate_normal(normal, p_heightmap, j, i, max_height);
            normals[vertex_pointer * 3]     = normal[0];
            normals[vertex_pointer * 3 + 1] = normal[1];
            normals[vertex_pointer * 3 + 2] = normal[2];
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
            indices[pointer++]     = top_left;
            indices[pointer++]     = bottom_left;
            indices[pointer++]     = top_right;
            indices[pointer++]     = top_right;
            indices[pointer++]     = bottom_left;
            indices[pointer++]     = bottom_right;
        }
    }

    RawModel * model
        = load_raw_model(vertices,
                         sizeof vertices / sizeof vertices[0],
                         texture_coords,
                         sizeof texture_coords / sizeof texture_coords[0],
                         indices,
                         sizeof indices / sizeof indices[0],
                         normals,
                         sizeof normals / sizeof normals[0],
                         NULL,
                         NULL,
                         0);
    return model;
}

/**
 * Create a new terrain with the provided heightmap and textures at (grid_x,
 * grid_z)
 * @param grid_x The grid x coordinate of the terrain
 * @param grid_z The grid z coordinate of the terrain
 * @param p_heightmap The heightmap to use for the terrain
 * @param p_texture_pack The textures to apply to the terrain
 * @param p_blend_map The blend map to apply the textures to the terrain
 * @return A pointer to the new terrain
 */
Terrain * create_terrain (const int                  grid_x,
                          const int                  grid_z,
                          const Image *              p_heightmap,
                          const TerrainTexturePack * p_texture_pack,
                          const Texture *            p_blend_map)
{
    Terrain * p_terrain = malloc(sizeof(*p_terrain));

    if (p_terrain == NULL)
    {
        nova_error("Failed to create terrain");
        return NULL;
    }

    const int VERTEX_COUNT = p_heightmap->height;

    p_terrain->texture_pack   = *p_texture_pack;
    p_terrain->blend_map      = *p_blend_map;
    p_terrain->max_height     = 40;
    p_terrain->x              = (float)(grid_x * TERRAIN_SIZE);
    p_terrain->z              = (float)(grid_z * TERRAIN_SIZE);
    p_terrain->grid_x         = grid_x;
    p_terrain->grid_z         = grid_z;
    p_terrain->heights_length = VERTEX_COUNT;
    p_terrain->pp_heights     = malloc(sizeof(float *) * VERTEX_COUNT);

    if (p_terrain->pp_heights == NULL)
    {
        nova_error("Failed to allocate terrain heights");
        return NULL;
    }

    for (int i = 0; i < VERTEX_COUNT; i++)
    {
        p_terrain->pp_heights[i] = malloc(sizeof(float) * VERTEX_COUNT);
        if (p_terrain->pp_heights[i] == NULL)
        {
            nova_error("Failed to allocate terrain heights");
            for (int j = 0; j < i; j++)
            {
                free(p_terrain->pp_heights[j]);
            }
            free(p_terrain->pp_heights);
            return NULL;
        }
    }

    RawModel * p_terrain_mesh = generate_terrain_mesh(
        p_heightmap, p_terrain->pp_heights, p_terrain->max_height);
    p_terrain->model = *p_terrain_mesh;

    free(p_terrain_mesh);

    return p_terrain;
}

void destroy_terrain (void * p_terrain)
{
    const Terrain * terrain = p_terrain;
    for (int i = 0; i < terrain->heights_length; i++)
    {
        free(terrain->pp_heights[i]);
    }
    free(terrain->pp_heights);
    free(p_terrain);
}

float terrain_get_height_at (const Terrain * p_terrain,
                             const float     world_x,
                             const float     world_z)
{
    const float terrain_x = world_x - p_terrain->x;
    const float terrain_z = world_z - p_terrain->z;
    const float grid_square_size
        = TERRAIN_SIZE / ((float)p_terrain->heights_length - 1.0f);
    const int grid_x = (int)floorf(terrain_x / grid_square_size);
    const int grid_z = (int)floorf(terrain_z / grid_square_size);

    if (grid_x >= p_terrain->heights_length - 1
        || grid_z >= p_terrain->heights_length - 1 || grid_x < 0 || grid_z < 0)
    {
        nova_info("OUT OF BOUNDS — returning 0");
        return 0;
    }

    const float x_coord = fmodf(terrain_x, grid_square_size) / grid_square_size;
    const float z_coord = fmodf(terrain_z, grid_square_size) / grid_square_size;
    float       height_at = 0.0f;
    if (x_coord <= (1 - z_coord))
    {
        height_at = barry_centric(
            (vec3) { 0.0f, p_terrain->pp_heights[grid_z][grid_x], 0.0f },
            (vec3) { 1.0f, p_terrain->pp_heights[grid_z][grid_x + 1], 0.0f },
            (vec3) { 0.0f, p_terrain->pp_heights[grid_z + 1][grid_x], 1.0f },
            (vec2) { x_coord, z_coord });
    }
    else
    {
        height_at = barry_centric(
            (vec3) { 1.0f, p_terrain->pp_heights[grid_z][grid_x + 1], 0.0f },
            (vec3) {
                1.0f, p_terrain->pp_heights[grid_z + 1][grid_x + 1], 1.0f },
            (vec3) { 0.0f, p_terrain->pp_heights[grid_z + 1][grid_x], 1.0f },
            (vec2) { x_coord, z_coord });
    }

    return height_at;
}

/**
 * Create a texture pack for a terrain
 * @param p_bg_texture The background of the terrain
 * @param p_r_texture The texture associated with the red value on the blend map
 * @param p_g_texture The texture associated with the green value on the blend
 * map
 * @param p_b_texture The texture associated with the blue value on the blend
 * map
 * @return
 */
TerrainTexturePack * create_terrain_texture_pack (Texture * p_bg_texture,
                                                  Texture * p_r_texture,
                                                  Texture * p_g_texture,
                                                  Texture * p_b_texture)
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

void destroy_terrain_texture_pack (TerrainTexturePack * p_texture_pack)
{
    free(p_texture_pack);
}