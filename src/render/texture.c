//
// Created by gregorym on 5/17/26.
//

#include <render/texture.h>
#include <gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "util/fileutils.h"
#include "util/novalogger.h"

/**
 * Creates a 2D texture with the specified width and height
 * @param p_texture_file_name The name of the texture file relative to the resources/textures folder
 * @param type The extension type of the texture, ex. PNG, JPG
 * @param width The width of the texture
 * @param height The height of the texture
 * @return The pointer to the newly created texture struct
 */
Texture * create_texture(const char * p_texture_file_name, const enum TextureType type)
{
    // Load texture file using STB
    //
    int32_t         width               = 0;
    int32_t         height              = 0;
    int32_t         nr_channels         = 0;
    const char *    p_texture_file_path = get_resource_location("textures", p_texture_file_name);
    unsigned char * p_data              = stbi_load(p_texture_file_path, &width, &height, &nr_channels, 0);

    if (p_data == NULL)
    {
        nova_error("Failed to load image %s", p_texture_file_path);
        return NULL;
    }

    // Create OpenGL texture
    //
    uint32_t texture_id = 0;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    // Texture parameters
    //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int32_t format = 0;

    switch (type)
    {
        case JPG: format = GL_RGB; break;
        case PNG: format = GL_RGBA; break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, p_data);
    // glGenerateMipmap(GL_TEXTURE_2D);

    // Free image data now that OpenGL texture is created
    //
    stbi_image_free(p_data);

    Texture * p_texture = malloc(sizeof(*p_texture));

    if (p_texture == NULL)
    {
        nova_error("Failed to allocate texture");
        return NULL;
    }

    p_texture->texture_id = texture_id;
    p_texture->width = width;
    p_texture->height = height;

    return p_texture;
}

// Free the resources taken by the texture
//
void destroy_texture(Texture * p_texture)
{
    glDeleteTextures(1, &p_texture->texture_id);
    free(p_texture);
}

/**
 * Binds a texture to the specified texture unit
 * @param p_texture The texture you want to bind
 * @param texture_unit The texture unit you want to bind to
 */
void bind_texture(const Texture * p_texture, const uint32_t texture_unit)
{
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_2D, p_texture->texture_id);
}

// Unbind the currently bound texture
void unbind_texture(void)
{
    glBindTexture(GL_TEXTURE_2D, 0);
}