//
// Created by gregorym on 5/30/26.
//

#ifndef NOVA_ENGINE_ENTITY_RENDERER_H
#define NOVA_ENGINE_ENTITY_RENDERER_H

#include <cglm.h>

#include "model/resource_loader.h"
#include "util/containers.h"

void init_entity_renderer(EntityShader * p_shader, mat4 projection_matrix);
void batch_render_entity(const HashMap * p_entity_map);
void render_loaded_model (const LoadedModel * p_loaded_model,
                          Entity *            p_entity);
void cleanup_entity_renderer(void);


#endif // NOVA_ENGINE_ENTITY_RENDERER_H
