//
// Created by gregorym on 5/18/26.
//

#ifndef NOVA_ENGINE_LIGHT_H
#define NOVA_ENGINE_LIGHT_H

#include <stdint.h>
#include <cglm.h>

typedef struct
{
    vec3 position;
    vec3 color;
} Light;

Light * create_light(vec3 position, vec3 color);
void    destroy_light(Light * p_light);
void    light_set_position(Light * light, vec3 position);
void    light_set_color(Light * light, vec3 color);

#endif // NOVA_ENGINE_LIGHT_H
