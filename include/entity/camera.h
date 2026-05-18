//
// Created by gregorym on 5/17/26.
//

#ifndef NOVA_ENGINE_CAMERA_H
#define NOVA_ENGINE_CAMERA_H

#include <cglm.h>

#include "entity.h"

typedef struct
{
    vec3  position;
    float yaw;
    float pitch;
    float roll;
    Entity * entity;
} Camera;

Camera * create_camera(Entity * p_follow_entity);
void     get_view_matrix(Camera * p_camera, mat4 dest);
void     camera_move(Camera * p_camera);

#endif //NOVA_ENGINE_CAMERA_H
