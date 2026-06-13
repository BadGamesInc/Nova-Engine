//
// Created by gregorym on 6/6/26.
//

#ifndef NOVA_ENGINE_TERRAIN_RENDERER_H
#define NOVA_ENGINE_TERRAIN_RENDERER_H

#include "shader.h"
#include "util/containers.h"

void init_terrain_renderer(TerrainShader * p_shader, mat4 projection_matrix);
void render_terrains(const ArrayList * p_terrains);
void cleanup_terrain_renderer(void);

#endif //NOVA_ENGINE_TERRAIN_RENDERER_H
