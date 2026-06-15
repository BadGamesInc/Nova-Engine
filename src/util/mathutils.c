//
// Created by gregorym on 5/17/26.
//

#include <util/mathutils.h>
#include "terrain/terrain.h"

/**
 * Creates a transformation matrix on give mat4
 * @param dest The mat4 to apply the properties to
 * @param translation How far to translate the matrix
 * @param rotation The rotation to apply in DEGREES
 * @param scale XYZ scale
 */
void create_transformation_matrix (mat4 dest,
                                   vec3 translation,
                                   vec3 rotation,
                                   vec3 scale)
{
    glm_mat4_identity(dest);
    glm_translate(dest, translation);
    glm_rotate(dest, glm_rad(rotation[0]), (vec3) { 1.0f, 0.0f, 0.0f });
    glm_rotate(dest, glm_rad(rotation[1]), (vec3) { 0.0f, 1.0f, 0.0f });
    glm_rotate(dest, glm_rad(rotation[2]), (vec3) { 0.0f, 0.0f, 1.0f });
    glm_scale(dest, scale);
}

float barry_centric (const vec3 p1,
                     const vec3 p2,
                     const vec3 p3,
                     const vec2 pos)
{
    const float det
        = (p2[2] - p3[2]) * (p1[0] - p3[0]) + (p3[0] - p2[0]) * (p1[2] - p3[2]);
    const float l1 = ((p2[2] - p3[2]) * (pos[0] - p3[0])
                      + (p3[0] - p2[0]) * (pos[1] - p3[2]))
                     / det;
    const float l2 = ((p3[2] - p1[2]) * (pos[0] - p3[0])
                      + (p1[0] - p3[0]) * (pos[1] - p3[2]))
                     / det;
    const float l3 = 1.0f - l1 - l2;
    return l1 * p1[1] + l2 * p2[1] + l3 * p3[1];
}

void get_grid_coords (vec2 dest, const float world_x, const float world_z)
{
    const int grid_x = (int)floorf(world_x / TERRAIN_SIZE);
    const int grid_z = (int)floorf(world_z / TERRAIN_SIZE);
    glm_vec2_copy((vec2) { (float)grid_x, (float)grid_z }, dest);
}
