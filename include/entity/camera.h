//
// Created by gregorym on 5/17/26.
//

#ifndef NOVA_ENGINE_CAMERA_H
#define NOVA_ENGINE_CAMERA_H

#include <cglm.h>

#include "entity.h"

typedef struct
{
    vec3     position;
    mat4     view_matrix;
    float    yaw;
    float    pitch;
    float    roll;
    float    angle_around_entity;
    float    distance_to_entity;
    float    target_angle_around_entity;
    float    target_distance_to_entity;
    float    target_real_pitch;
    Entity * entity;
} Camera;

Camera * create_camera(Entity * p_follow_entity);
void     destroy_camera(Camera * p_camera);
void     get_view_matrix(Camera * p_camera, mat4 dest);
void     camera_move(Camera * p_camera);

#endif // NOVA_ENGINE_CAMERA_H
