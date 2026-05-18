//
// Created by gregorym on 5/10/26.
//

#ifndef NOVA_ENGINE_SHADER_H
#define NOVA_ENGINE_SHADER_H

#include <stdint.h>
#include <cglm.h>

typedef struct
{
    uint32_t program_id;
} Shader;

Shader * create_shader (const char * p_vertex_file_name, const char * p_fragment_file_name);
void     bind_shader (const Shader * p_shader);
void     unbind_shader (void);
void     destroy_shader (Shader * p_shader);
void     uniform_1f (const Shader * p_shader, const char * p_uniform_name, float value);
void     uniform_3f (const Shader * p_shader, const char * p_uniform_name, const vec3 value);
void     uniform_mat4 (const Shader * p_shader, const char * p_uniform_name, const mat4 value);

#endif //NOVA_ENGINE_SHADER_H
