//
// Created by gregorym on 5/10/26.
//

#ifndef NOVA_ENGINE_RESOURCELOADER_H
#define NOVA_ENGINE_RESOURCELOADER_H

#include <render/model/model.h>
#include <stdint.h>

#include "render/texture.h"

RawModel * load_raw_model (const float * vertices, int32_t vertices_length,
                            const float * texture_coords, int32_t texture_coords_length,
                            const uint32_t * indices, int32_t indices_length);
Texture *  load_texture(const char * texture_name, enum TextureType texture_type);
void       clean_resources (void);
void       init_resource_loader (void);
RawModel * load_from_obj_file (const char * p_obj_filename);

#endif //NOVA_ENGINE_RESOURCELOADER_H
