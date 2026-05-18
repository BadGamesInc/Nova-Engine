//
// Created by gregorym on 5/17/26.
//

#include <util/mathutils.h>

/**
 * Creates a transformation matrix on give mat4
 * @param dest The mat4 to apply the properties to
 * @param translation How far to translate the matrix
 * @param rotation The rotation to apply
 * @param scale XYZ scale
 */
void create_transformation_matrix (mat4 dest, vec3 translation, vec3 rotation, vec3 scale)
{
    glm_mat4_identity(dest);
    glm_translate(dest, translation);
    glm_rotate(dest, rotation[0], (vec3) {1.0f, 0.0f, 0.0f});
    glm_rotate(dest, rotation[1], (vec3) {0.0f, 1.0f, 0.0f});
    glm_rotate(dest, rotation[2], (vec3) {0.0f, 0.0f, 1.0f});
    glm_scale(dest, scale);
}