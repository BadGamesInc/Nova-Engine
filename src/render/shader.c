//
// Created by gregorym on 5/10/26.
//

#include <render/shader.h>
#include <util/fileutils.h>
#include <gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "util/novalogger.h"

/**
 * Create and link a shader with the provided vertex and fragment shader files
 * @param p_vertex_file_name The name of the vertex shader file
 * @param p_fragment_file_name The name of the fragment shader file
 * @return The pointer to the newly created Shader
 */
Shader * create_shader (const char * p_vertex_file_name, const char * p_fragment_file_name)
{
    // Create vertex shader
    //
    const uint32_t vertex_shader        = glCreateShader(GL_VERTEX_SHADER);
    char *         p_vertex_path          = get_resource_location("shaders", p_vertex_file_name);
    char *         p_vertex_shader_source = load_file_to_string(p_vertex_path);
    free(p_vertex_path);

    glShaderSource(vertex_shader, 1, (const GLchar**) &p_vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    free(p_vertex_shader_source);

    // Check if vertex shader compiled correctly
    //
    int32_t  success       = 0;
    char     info_log[512] = {0};
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        nova_error("Vertex shader '%s' failed to compile\n%s", p_vertex_file_name, info_log);
        return NULL;
    }

    // Create fragment shader
    //
    const uint32_t fragment_shader        = glCreateShader(GL_FRAGMENT_SHADER);
    char *         p_fragment_path          = get_resource_location("shaders", p_fragment_file_name);
    char *         p_fragment_shader_source = load_file_to_string(p_fragment_path);
    free(p_fragment_path);

    glShaderSource(fragment_shader, 1, (const GLchar**) &p_fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    free(p_fragment_shader_source);

    // Check if fragment shader compiled correctly
    //
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        nova_error("Fragment shader '%s' failed to compile\n%s", p_fragment_file_name, info_log);
        glDeleteShader(vertex_shader);
        return NULL;
    }

    // Link the vertex and fragment shader
    //
    const uint32_t program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader);
    glAttachShader(program_id, fragment_shader);
    glLinkProgram(program_id);

    // Check link status of shader program
    //
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(program_id, 512, NULL, info_log);
        nova_error("Failed to link shader program\n%s", info_log);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return NULL;
    }

    // Delete vertex and fragment shaders as they are already linked
    //
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    Shader * p_shader = malloc(sizeof(*p_shader));

    if (p_shader == NULL)
    {
        nova_error("Failed to create shader pointer");
        return NULL;
    }

    p_shader->program_id = program_id;

    return p_shader;
}

// Bind the specified shader for use
//
void bind_shader (const Shader * p_shader)
{
    glUseProgram(p_shader->program_id);
}

// Unbind currently bound shader
//
void unbind_shader (void)
{
    glUseProgram(0);
}

// Free memory from specified shader
//
void destroy_shader (Shader * p_shader)
{
    glDeleteProgram(p_shader->program_id);
    free(p_shader);
}

void uniform_1f (const Shader * p_shader, const char * p_uniform_name, const float value)
{
    glUniform1f(glGetUniformLocation(p_shader->program_id, p_uniform_name), value);
}

void uniform_3f (const Shader * p_shader, const char * p_uniform_name, const vec3 value)
{
    glUniform3fv(glGetUniformLocation(p_shader->program_id, p_uniform_name), 1, value);
}

void uniform_mat4 (const Shader * p_shader, const char * p_uniform_name, const mat4 value)
{
    glUniformMatrix4fv(glGetUniformLocation(p_shader->program_id, p_uniform_name), 1, GL_FALSE, value[0]);
}