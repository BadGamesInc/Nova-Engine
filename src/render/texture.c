//
// Created by gregorym on 5/17/26.
//

#include <render/texture.h>
#include <gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "util/fileutils.h"
#include "util/nova_logger.h"

#define LOD_BIAS (-0.5)

static Texture * upload_texture (unsigned char * p_data,
                                 int             width,
                                 int             height,
                                 int             nr_channels)
{
    int format = 0;

    switch (nr_channels)
    {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            nova_error("Unsupported channel count: %d", nr_channels);
            stbi_image_free(p_data);
            return NULL;
    }

    // Create OpenGL texture
    //
    unsigned int texture_id = 0;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // Texture parameters
    //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, LOD_BIAS);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 format,
                 width,
                 height,
                 0,
                 format,
                 GL_UNSIGNED_BYTE,
                 p_data);

    glGenerateMipmap(GL_TEXTURE_2D);

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
    p_texture->width      = width;
    p_texture->height     = height;
    p_texture->num_rows   = 1;

    return p_texture;
}

/**
 * Creates a 2D texture with the specified width and height
 * @param p_texture_file_name The name of the texture file relative to the
 * resources/textures folder
 * @return The pointer to the newly created texture struct
 */
Texture * create_texture (const char * p_texture_file_name)
{
    // Load texture file using STB
    //
    int          width       = 0;
    int          height      = 0;
    int          nr_channels = 0;
    const char * p_texture_file_path
        = get_resource_location("textures", p_texture_file_name);
    unsigned char * p_data
        = stbi_load(p_texture_file_path, &width, &height, &nr_channels, 0);

    if (p_data == NULL)
    {
        nova_error("Failed to load image %s", p_texture_file_path);
        return NULL;
    }

    return upload_texture(p_data, width, height, nr_channels);
}

/**
 * Creates a 2D texture with the specified width and height from an absolute
 * path
 * @param p_absolute_path The absolute path of the image
 * @return The pointer to the newly created texture struct
 */
Texture * create_texture_from_path (const char * p_absolute_path)
{
    // Load texture file using STB
    //
    int             width       = 0;
    int             height      = 0;
    int             nr_channels = 0;
    unsigned char * p_data
        = stbi_load(p_absolute_path, &width, &height, &nr_channels, 0);

    if (p_data == NULL)
    {
        nova_error("Failed to load image %s", p_absolute_path);
        return NULL;
    }

    return upload_texture(p_data, width, height, nr_channels);
}

Texture * load_texture_from_memory (unsigned char * p_data, int size)
{
    int             width       = 0;
    int             height      = 0;
    int             nr_channels = 0;
    unsigned char * p_pixels
        = stbi_load_from_memory(p_data, size, &width, &height, &nr_channels, 0);

    if (p_pixels == NULL)
    {
        nova_error("Failed to decode embedded texture: %s",
                   stbi_failure_reason());
        return NULL;
    }

    return upload_texture(p_pixels, width, height, nr_channels);
}

// Free the resources taken by the texture
//
void destroy_texture (Texture * p_texture)
{
    glDeleteTextures(1, &p_texture->texture_id);
    free(p_texture);
}

/**
 * Binds a texture to the specified texture unit
 * @param p_texture The texture you want to bind
 * @param texture_unit The texture unit you want to bind to
 */
void bind_texture (const Texture * p_texture, const unsigned int texture_unit)
{
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_2D, p_texture->texture_id);
}

// Unbind the currently bound texture
void unbind_texture (void)
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * Load a non-OpenGL image to read individual pixels
 * @param p_image_file_name The file name of the image
 * @return A pointer to the Image
 */
Image * load_image (const char * p_image_file_name)
{
    Image * p_image = malloc(sizeof(*p_image));

    if (p_image == NULL)
    {
        nova_error("Failed to allocate Image");
        return NULL;
    }

    int          width    = 0;
    int          height   = 0;
    int          channels = 0;
    const char * p_image_file_path
        = get_resource_location("textures", p_image_file_name);
    p_image->data = stbi_load(p_image_file_path, &width, &height, &channels, 0);

    if (p_image->data == NULL)
    {
        nova_error("Failed to load image %s", p_image_file_name);
        free(p_image);
        return NULL;
    }

    p_image->width    = width;
    p_image->height   = height;
    p_image->channels = channels;

    return p_image;
}

/**
 * Free the image and its data
 * @param p_image The image to free
 */
void destroy_image (Image * p_image)
{
    stbi_image_free(p_image->data);
    free(p_image);
}

/**
 * Get the rgb value of a specific coordinate in the provided image and store it
 * in a vec3
 * @param p_image The image to read from
 * @param dest The vec3 to store the rgba in
 * @param x The x coordinate to read
 * @param y The y coordinate to read
 */
void image_get_rgb (const Image * p_image, vec3 dest, const int x, const int y)
{
    if (p_image->channels != 3)
    {
        nova_error("Image is not a valid RGB image");
        return;
    }

    const size_t index = (size_t)p_image->channels * (y * p_image->width + x);

    dest[0] = p_image->data[index];     // Red
    dest[1] = p_image->data[index + 1]; // Green
    dest[2] = p_image->data[index + 2]; // Blue
}

/**
 * Get the rgba value of a specific coordinate in the provided image and store
 * it in a vec4
 * @param p_image The image to read from
 * @param dest The vec4 to store the rgba in
 * @param x The x coordinate to read
 * @param y The y coordinate to read
 */
void image_get_rgba (const Image * p_image, vec4 dest, const int x, const int y)
{
    if (p_image->channels != 4)
    {
        nova_error("Image does not have an alpha channel");
        return;
    }

    const size_t index = (size_t)p_image->channels * (y * p_image->width + x);

    dest[0] = p_image->data[index];     // Red
    dest[1] = p_image->data[index + 1]; // Green
    dest[2] = p_image->data[index + 2]; // Blue
    dest[3] = p_image->data[index + 3]; // Alpha
}