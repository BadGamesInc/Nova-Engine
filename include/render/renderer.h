//
// Created by gregorym on 5/10/26.
//

#ifndef NOVA_ENGINE_RENDERER_H
#define NOVA_ENGINE_RENDERER_H

#define FOV          70
#define NEAR_PLANE   0.1
#define FAR_PLANE    1000
#define FOG_DENSITY  0.0035f
#define FOG_GRADIENT 5.0f
#define SKY_COLOR    ((vec3) {0.5f, 0.5f, 0.6f})

#include "entity/entity.h"
#include "entity/camera.h"
#include "entity/light.h"
#include "terrain/terrain.h"

void renderer_init(void);
void enable_culling(void);
void disable_culling(void);
void unbind_model (void);
void process_entity(Entity * entity);
void process_terrain (Terrain * p_terrain);
void renderer_render_main(Camera * p_camera, const Light * p_light);
void renderer_cleanup(void);

#endif // NOVA_ENGINE_RENDERER_H
