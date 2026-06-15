//
// Created by gregorym on 5/17/26.
//

#include "entity/camera.h"

#include "GLFW/glfw3.h"
#include "util/nova_logger.h"
#include "util/input.h"
#include "util/mathutils.h"

#define SMOOTHING_FACTOR 0.5f
#define ZOOM_SMOOTHING   5.0f
#define ROTATE_SMOOTHING 5.0f

static double last_mouse_x = 0.0;
static double last_mouse_y = 0.0;

/**
 * Creates a new camera that follows the provided entity
 * @param p_follow_entity The entity the camera should follow
 * @return A pointer to the new Camera
 */
Camera * create_camera (Entity * p_follow_entity)
{
    Camera * camera = malloc(sizeof(*camera));

    if (camera == NULL)
    {
        nova_error("Failed to allocate Camera");
        return NULL;
    }

    camera->entity                     = p_follow_entity;
    camera->yaw                        = 0.0f;
    camera->pitch                      = 0.0f;
    camera->roll                       = 0.0f;
    camera->angle_around_entity        = 0.0f;
    camera->distance_to_entity         = 30.0f;
    camera->target_angle_around_entity = 0.0f;
    camera->target_distance_to_entity  = 30.0f;
    camera->target_real_pitch          = 0.0f;
    glm_vec3_copy((vec3) { 0.0f, 0.0f, 0.0f }, camera->position);

    return camera;
}

void destroy_camera (Camera * p_camera)
{
    free(p_camera);
}

void get_view_matrix (Camera * p_camera, mat4 dest)
{
    glm_mat4_identity(dest);
    glm_rotate(dest, glm_rad(p_camera->pitch), (vec3) { 1.0f, 0.0f, 0.0f });
    glm_rotate(dest, glm_rad(p_camera->yaw), (vec3) { 0.0f, 1.0f, 0.0f });
    glm_rotate(dest, glm_rad(p_camera->roll), (vec3) { 0.0f, 0.0f, 1.0f });
    vec3 cam_pos = GLM_VEC3_ZERO_INIT;
    glm_vec3_copy(p_camera->position, cam_pos);
    vec3 neg_cam_pos = { -cam_pos[0], -cam_pos[1], -cam_pos[2] };
    glm_translate(dest, neg_cam_pos);
}

void calculate_pitch (Camera * p_camera)
{
    const double mouse_dy = get_mouse_y() - last_mouse_y;

    if (is_mouse_button_pressed(GLFW_MOUSE_BUTTON_1))
    {
        const float pitch_delta = (float)mouse_dy * 0.2f;
        p_camera->pitch += pitch_delta;

        glm_clamp(p_camera->pitch, 0.0f, 90.0f);
    }

    last_mouse_y = get_mouse_y();
}

void calculate_zoom (Camera * p_camera)
{
    float       target_zoom = p_camera->target_distance_to_entity;
    const float zoom_level  = (float)get_mouse_scroll_y() * 2.8f;
    target_zoom -= zoom_level;
    target_zoom                         = glm_max(target_zoom, 1.0f);
    p_camera->target_distance_to_entity = target_zoom;
    const float offset
        = p_camera->target_distance_to_entity - p_camera->distance_to_entity;
    p_camera->distance_to_entity += offset * 0.01f * ZOOM_SMOOTHING;
    update_mouse_scroll(0, 0);
}

void calculate_angle_around_entity (Camera * p_camera)
{
    const double mouse_dx = get_mouse_x() - last_mouse_x;

    if (is_mouse_button_pressed(GLFW_MOUSE_BUTTON_1))
    {
        const float angle_delta = (float)mouse_dx * 0.3f;
        p_camera->target_angle_around_entity -= angle_delta;
    }

    last_mouse_x = get_mouse_x();
    const float offset
        = p_camera->target_angle_around_entity - p_camera->angle_around_entity;
    p_camera->angle_around_entity += offset * 0.01f * ROTATE_SMOOTHING;
}

void calculate_camera_pos (Camera *    p_camera,
                           const float h_distance,
                           const float v_distance)
{
    const float theta    = p_camera->angle_around_entity;
    const float offset_x = h_distance * sinf(glm_rad(theta));
    const float offset_z = h_distance * cosf(glm_rad(theta));

    vec3 entity_pos;
    glm_vec3_copy(p_camera->entity->position, entity_pos);

    p_camera->position[0] = entity_pos[0] - offset_x;
    p_camera->position[1] = entity_pos[1] + v_distance + 2.0f;
    p_camera->position[2] = entity_pos[2] - offset_z;
}

void update_view_matrix (Camera * p_camera)
{
    glm_mat4_identity(p_camera->view_matrix);
    glm_rotate(p_camera->view_matrix,
               glm_rad(p_camera->pitch),
               (vec3) { 1.0f, 0.0f, 0.0f });
    glm_rotate(p_camera->view_matrix,
               glm_rad(p_camera->yaw),
               (vec3) { 0.0f, 1.0f, 0.0f });
    glm_rotate(p_camera->view_matrix,
               glm_rad(p_camera->roll),
               (vec3) { 0.0f, 0.0f, 1.0f });
    vec3 cam_pos = GLM_VEC3_ZERO_INIT;
    glm_vec3_copy(p_camera->position, cam_pos);
    vec3 neg_cam_pos = { -cam_pos[0], -cam_pos[1], -cam_pos[2] };
    glm_translate(p_camera->view_matrix, neg_cam_pos);
}

void camera_move (Camera * p_camera)
{
    calculate_pitch(p_camera);
    calculate_angle_around_entity(p_camera);
    calculate_zoom(p_camera);
    float horizontal_distance
        = p_camera->distance_to_entity * cosf(glm_rad(p_camera->pitch));
    float vertical_distance
        = p_camera->distance_to_entity * sinf(glm_rad(p_camera->pitch));
    calculate_camera_pos(p_camera, horizontal_distance, vertical_distance);
    p_camera->yaw = 180.0f - (p_camera->angle_around_entity);
    p_camera->yaw = fmodf(p_camera->yaw, 360.0f);
    update_view_matrix(p_camera);
}
