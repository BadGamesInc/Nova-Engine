//
// Created by gregorym on 5/17/26.
//

#ifndef NOVA_ENGINE_TEXTURE_H
#define NOVA_ENGINE_TEXTURE_H

#include <cglm.h>

typedef struct
{
    int          width;
    int          height;
    int          num_rows;
    unsigned int texture_id;
} Texture;

typedef struct
{
    int             width;
    int             height;
    int             channels;
    unsigned char * data;
} Image;

Texture * create_texture(const char * p_texture_file_name);
Texture * create_texture_from_path(const char * p_absolute_path);
Texture * load_texture_from_memory(unsigned char * p_data, int size);
void      destroy_texture(Texture * p_texture);
void      bind_texture(const Texture * p_texture, unsigned int texture_unit);
void      unbind_texture(void);
Image *   load_image(const char * p_image_file_name);
void      destroy_image(Image * p_image);
void      image_get_rgb(const Image * p_image, vec3 dest, int x, int y);
void      image_get_rgba(const Image * p_image, vec4 dest, int x, int y);

#endif // NOVA_ENGINE_TEXTURE_H
