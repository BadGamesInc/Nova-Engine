//
// Created by gregorym on 5/17/26.
//

#ifndef NOVA_ENGINE_MATHUTILS_H
#define NOVA_ENGINE_MATHUTILS_H

#include <cglm.h>

#define PI 3.14159

void  create_transformation_matrix(mat4 dest,
                                   vec3 translation,
                                   vec3 rotation,
                                   vec3 scale);
float barry_centric(const vec3 p1,
                    const vec3 p2,
                    const vec3 p3,
                    const vec2 pos);
void  get_grid_coords(vec2 dest, float world_x, float world_z);

#endif // NOVA_ENGINE_MATHUTILS_H
