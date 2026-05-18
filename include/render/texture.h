//
// Created by gregorym on 5/17/26.
//

#ifndef NOVA_ENGINE_TEXTURE_H
#define NOVA_ENGINE_TEXTURE_H

#include <stdint.h>

typedef struct
{
    uint32_t width;
    uint32_t height;
    uint32_t texture_id;
} Texture;

enum TextureType
{
    JPG,
    PNG
};

Texture * create_texture(const char * p_texture_file_name, enum TextureType type);
void      destroy_texture(Texture * p_texture);
void      bind_texture(const Texture * p_texture, uint32_t texture_unit);
void      unbind_texture(void);

#endif //NOVA_ENGINE_TEXTURE_H
