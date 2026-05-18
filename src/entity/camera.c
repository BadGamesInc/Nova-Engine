//
// Created by gregorym on 5/17/26.
//

#include "entity/camera.h"
#include "util/novalogger.h"

Camera * create_camera (Entity * p_follow_entity)
{
    Camera * camera = malloc (sizeof (*camera));

    if (camera == NULL)
    {
        nova_error("Failed to allocate Camera");
        return NULL;
    }

    camera->entity = p_follow_entity;
    camera->yaw = 0.0f;
    camera->pitch = 0.0f;
    camera->roll = 0.0f;
    glm_vec3_copy(p_follow_entity->position, camera->position);

    return camera;
}

void get_view_matrix(Camera * p_camera, mat4 dest)
{
    glm_mat4_identity(dest);
    glm_rotate(dest, glm_rad(p_camera->pitch), (vec3) {1.0f, 0.0f, 0.0f});
    glm_rotate(dest, glm_rad(p_camera->yaw), (vec3) {0.0f, 1.0f, 0.0f});
    glm_rotate(dest, glm_rad(p_camera->roll), (vec3) {0.0f, 0.0f, 1.0f});
    vec3 cam_pos = GLM_VEC3_ZERO_INIT;
    glm_vec3_copy(p_camera->position, cam_pos);
    vec3 neg_cam_pos = {-cam_pos[0], -cam_pos[1], -cam_pos[2]};
    glm_translate(dest, neg_cam_pos);
}

void camera_move(Camera * p_camera)
{
    //p_camera->pitch -= 0.25f;
}
