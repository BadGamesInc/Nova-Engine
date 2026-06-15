//
// Created by gregorym on 5/10/26.
//

#include <render/shader.h>
#include <util/fileutils.h>
#include <gl.h>
#include <stdlib.h>
#include <stdint.h>

#include "util/nova_logger.h"

/**
 * Create and link a shader with the provided vertex and fragment shader files
 * @param p_vertex_file_name The name of the vertex shader file
 * @param p_fragment_file_name The name of the fragment shader file
 * @return The pointer to the newly created Shader
 */
Shader * create_shader (const char * p_vertex_file_name,
                        const char * p_fragment_file_name)
{
    // Create vertex shader
    //
    const unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    char * p_vertex_path = get_resource_location("shaders", p_vertex_file_name);
    char * p_vertex_shader_source = load_file_to_string(p_vertex_path);
    free(p_vertex_path);

    glShaderSource(
        vertex_shader, 1, (const GLchar **)&p_vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    free(p_vertex_shader_source);

    // Check if vertex shader compiled correctly
    //
    int  success       = 0;
    char info_log[512] = { 0 };
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        nova_error("Vertex shader '%s' failed to compile\n%s",
                   p_vertex_file_name,
                   info_log);
        return NULL;
    }

    // Create fragment shader
    //
    const unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    char *             p_fragment_path
        = get_resource_location("shaders", p_fragment_file_name);
    char * p_fragment_shader_source = load_file_to_string(p_fragment_path);
    free(p_fragment_path);

    glShaderSource(
        fragment_shader, 1, (const GLchar **)&p_fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    free(p_fragment_shader_source);

    // Check if fragment shader compiled correctly
    //
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        nova_error("Fragment shader '%s' failed to compile\n%s",
                   p_fragment_file_name,
                   info_log);
        glDeleteShader(vertex_shader);
        return NULL;
    }

    // Link the vertex and fragment shader
    //
    const unsigned int program_id = glCreateProgram();
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

// Load a single float value to shader
//
void shader_uniform_1f (const Shader * p_shader,
                        const char *   p_uniform_name,
                        const float    value)
{
    glUniform1f(glGetUniformLocation(p_shader->program_id, p_uniform_name),
                value);
}

// Load a vec2f to shader
void shader_uniform_2f (const Shader * p_shader,
                        const char *   p_uniform_name,
                        const vec2     value)
{
    glUniform2fv(
        glGetUniformLocation(p_shader->program_id, p_uniform_name), 1, value);
}

// Load a vec3f to the shader
//
void shader_uniform_3f (const Shader * p_shader,
                        const char *   p_uniform_name,
                        const vec3     value)
{
    glUniform3fv(
        glGetUniformLocation(p_shader->program_id, p_uniform_name), 1, value);
}

// Load a 4x4 matrix to the shader
//
void shader_uniform_mat4 (const Shader * p_shader,
                          const char *   p_uniform_name,
                          const mat4     value)
{
    glUniformMatrix4fv(
        glGetUniformLocation(p_shader->program_id, p_uniform_name),
        1,
        GL_FALSE,
        value[0]);
}

void shader_uniform_bool (const Shader * p_shader,
                          const char *   p_uniform_name,
                          const bool     value)
{
    glUniform1i(glGetUniformLocation(p_shader->program_id, p_uniform_name),
                (int)value);
}

void shader_uniform_1i (const Shader * p_shader,
                        const char *   p_uniform_name,
                        const int      value)
{
    glUniform1i(glGetUniformLocation(p_shader->program_id, p_uniform_name),
                value);
}

/**
 * Creates the shader used for entities
 * @return Pointer to new EntityShader
 */
EntityShader * create_entity_shader (void)
{
    Shader *       p_shader = create_shader("entity.vert", "entity.frag");
    EntityShader * p_entity_shader = malloc(sizeof(*p_entity_shader));

    if (p_entity_shader == NULL)
    {
        nova_error("Failed to create entity shader");
        destroy_shader(p_shader);
        return NULL;
    }

    p_entity_shader->shader = *p_shader;

    free(p_shader);

    return p_entity_shader;
}

// Free memory used from EntityShader
//
void destroy_entity_shader (EntityShader * p_shader)
{
    glDeleteProgram(p_shader->shader.program_id);
    free(p_shader);
}

// Load the light values to entity shader
//
void entity_shader_load_light (EntityShader * p_entity_shader,
                               const Light *  p_light)
{
    shader_uniform_3f(
        (Shader *)p_entity_shader, "light_position", p_light->position);
    shader_uniform_3f((Shader *)p_entity_shader, "light_color", p_light->color);
}

// Load shine/reflectivity variables to fragment shader
//
void entity_shader_load_shine_values (EntityShader * p_entity_shader,
                                      const float    shine_damper,
                                      const float    reflectivity)
{
    shader_uniform_1f((Shader *)p_entity_shader, "shine_damper", shine_damper);
    shader_uniform_1f((Shader *)p_entity_shader, "reflectivity", reflectivity);
}

// Load the fog values to the shader
//
void entity_shader_load_fog_values (EntityShader * p_entity_shader,
                                    const float    density,
                                    const float    gradient,
                                    vec3           color)
{
    shader_uniform_1f((Shader *)p_entity_shader, "fog_density", density);
    shader_uniform_1f((Shader *)p_entity_shader, "fog_gradient", gradient);
    shader_uniform_3f((Shader *)p_entity_shader, "sky_color", color);
}

void entity_shader_load_tex_rows (EntityShader * p_entity_shader,
                                  const float    rows)
{
    shader_uniform_1f((Shader *)p_entity_shader, "num_rows", rows);
}

void entity_shader_load_tex_offset (EntityShader * p_entity_shader,
                                    const float    x_offset,
                                    const float    y_offset)
{
    shader_uniform_2f(
        (Shader *)p_entity_shader, "tex_offset", (vec2) { x_offset, y_offset });
}

/**
 * Create a new terrain shader and return the pointer
 * @return A pointer to the new terrain shader
 */
TerrainShader * create_terrain_shader (void)
{
    Shader *        p_shader = create_shader("terrain.vert", "terrain.frag");
    TerrainShader * p_terrain_shader = malloc(sizeof(*p_terrain_shader));

    if (p_terrain_shader == NULL)
    {
        nova_error("Failed to create terrain shader");
        destroy_shader(p_shader);
        return NULL;
    }

    p_terrain_shader->shader = *p_shader;
    free(p_shader);

    return p_terrain_shader;
}

void destroy_terrain_shader (TerrainShader * p_shader)
{
    glDeleteProgram(p_shader->shader.program_id);
    free(p_shader);
}

// Load the light values to entity shader
//
void terrain_shader_load_light (TerrainShader * p_entity_shader,
                                const Light *   p_light)
{
    shader_uniform_3f(
        (Shader *)p_entity_shader, "light_position", p_light->position);
    shader_uniform_3f((Shader *)p_entity_shader, "light_color", p_light->color);
}

// Load shine/reflectivity variables to fragment shader
//
void terrain_shader_load_shine_values (TerrainShader * p_entity_shader,
                                       const float     shine_damper,
                                       const float     reflectivity)
{
    shader_uniform_1f((Shader *)p_entity_shader, "shine_damper", shine_damper);
    shader_uniform_1f((Shader *)p_entity_shader, "reflectivity", reflectivity);
}

// Load the fog values to the shader
//
void terrain_shader_load_fog_values (TerrainShader * p_terrain_shader,
                                     const float     density,
                                     const float     gradient,
                                     vec3            color)
{
    shader_uniform_1f((Shader *)p_terrain_shader, "fog_density", density);
    shader_uniform_1f((Shader *)p_terrain_shader, "fog_gradient", gradient);
    shader_uniform_3f((Shader *)p_terrain_shader, "sky_color", color);
}

// Send the appropriate terrain textures to the correct texture units in the
// shader
//
void terrain_shader_connect_texture_units (TerrainShader * p_terrain_shader)
{
    shader_uniform_1i((Shader *)p_terrain_shader, "background_texture", 0);
    shader_uniform_1i((Shader *)p_terrain_shader, "r_texture", 1);
    shader_uniform_1i((Shader *)p_terrain_shader, "g_texture", 2);
    shader_uniform_1i((Shader *)p_terrain_shader, "b_texture", 3);
    shader_uniform_1i((Shader *)p_terrain_shader, "blend_map", 4);
}

FontShader * create_font_shader (void)
{
    Shader *     p_shader      = create_shader("font.vert", "font.frag");
    FontShader * p_font_shader = malloc(sizeof(*p_font_shader));

    p_font_shader->shader = *p_shader;
    free(p_shader);

    return p_font_shader;
}

void destroy_font_shader(void * p_shader)
{
    free(p_shader);
}