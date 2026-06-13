//
// Created by gregorym on 5/10/26.
//

#ifndef NOVA_ENGINE_SHADER_H
#define NOVA_ENGINE_SHADER_H

#include <stdint.h>
#include <cglm.h>

#include "entity/light.h"

typedef struct
{
    unsigned int program_id;
} Shader;

typedef struct
{
    Shader shader;
} EntityShader;

typedef struct
{
    Shader shader;
} TerrainShader;

Shader * create_shader(const char * p_vertex_file_name,
                       const char * p_fragment_file_name);
void     bind_shader(const Shader * p_shader);
void     unbind_shader(void);
void     destroy_shader(Shader * p_shader);
void     shader_uniform_1f(const Shader * p_shader,
                           const char *   p_uniform_name,
                           float          value);
void     shader_uniform_3f(const Shader * p_shader,
                           const char *   p_uniform_name,
                           const vec3     value);
void     shader_uniform_mat4(const Shader * p_shader,
                             const char *   p_uniform_name,
                             const mat4     value);
void     shader_uniform_bool(const Shader * p_shader,
                            const char *    p_uniform_name,
                            bool            value
                         );
void     shader_uniform_1i(const Shader * p_shader,
                           const char *   p_uniform_name,
                           int            value);

// Entity Shader
//
EntityShader * create_entity_shader(void);
void           destroy_entity_shader(EntityShader * p_shader);
void           entity_shader_load_light(EntityShader * p_entity_shader,
                                        const Light *  p_light);
void           entity_shader_load_shine_values(EntityShader * p_entity_shader,
                                             float          shine_damper,
                                             float          reflectivity);
void           entity_shader_load_fog_values(EntityShader * p_entity_shader,
                                             float          density,
                                             float          gradient,
                                             vec3           color);

// Terrain Shader
//
TerrainShader * create_terrain_shader(void);
void           destroy_terrain_shader(TerrainShader * p_shader);
void           terrain_shader_load_light(TerrainShader * p_entity_shader,
                                         const Light *   p_light);
void           terrain_shader_load_shine_values(TerrainShader * p_entity_shader,
                                                float           shine_damper,
                                                float           reflectivity);
void           terrain_shader_load_fog_values(TerrainShader * p_terrain_shader,
                                              float           density,
                                              float           gradient,
                                              vec3            color);
void           terrain_shader_connect_texture_units(TerrainShader * p_terrain_shader);

#endif // NOVA_ENGINE_SHADER_H
