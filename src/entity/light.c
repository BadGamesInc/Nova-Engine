//
// Created by gregorym on 5/18/26.
//

#include "entity/light.h"

#include "util/nova_logger.h"

/**
 * Create a new light with the given position and color
 * @param position The position of the light
 * @param color The color of the light
 * @return Pointer to the new light
 */
Light * create_light (vec3 position, vec3 color)
{
    Light * light = malloc(sizeof(*light));

    if (light == NULL)
    {
        nova_error("Failed to allocate Light");
        return NULL;
    }

    glm_vec3_copy(position, light->position);
    glm_vec3_copy(color, light->color);

    return light;
}

void destroy_light (Light * p_light)
{
    free(p_light);
}

// Set the position of the given light
//
void light_set_position (Light * light, vec3 position)
{
    glm_vec3_copy(position, light->position);
}

// Set the color of the given light
//
void light_set_color (Light * light, vec3 color)
{
    glm_vec3_copy(color, light->color);
}