//
// Created by gregorym on 5/10/26.
//

#include <gl.h>
#include <render/renderer.h>
#include <render/model/model.h>
#include <render/model/resourceloader.h>

#include <render/shader.h>
#include <cglm.h>
#include <entity/entity.h>
#include <util/mathutils.h>

#include "engine.h"
#include "entity/camera.h"

// Global variables
//
RawModel *      gp_model          = NULL;
Texture *       gp_texture        = NULL;
ModelTexture *  gp_model_texture  = NULL;
TexturedModel * gp_textured_model = NULL;
Entity *        gp_entity         = NULL;
Camera *        gp_camera         = NULL;
Shader *        gp_shader         = NULL;
mat4            g_projection_mat  = {{0.0f}};

void renderer_init (void)
{
    init_resource_loader();

    float vertices[72] = {
        -0.5f,0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f,0.5f,-0.5f,

        -0.5f,0.5f,0.5f,
        -0.5f,-0.5f,0.5f,
        0.5f,-0.5f,0.5f,
        0.5f,0.5f,0.5f,

        0.5f,0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,0.5f,
        0.5f,0.5f,0.5f,

        -0.5f,0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f,0.5f,
        -0.5f,0.5f,0.5f,

        -0.5f,0.5f,0.5f,
        -0.5f,0.5f,-0.5f,
        0.5f,0.5f,-0.5f,
        0.5f,0.5f,0.5f,

        -0.5f,-0.5f,0.5f,
        -0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,0.5f
};

    float texcoords[48] = {
        0,0,
        0,1,
        1,1,
        1,0,
        0,0,
        0,1,
        1,1,
        1,0,
        0,0,
        0,1,
        1,1,
        1,0,
        0,0,
        0,1,
        1,1,
        1,0,
        0,0,
        0,1,
        1,1,
        1,0,
        0,0,
        0,1,
        1,1,
        1,0
};

    int indices[36] = {
        0,1,3,
        3,1,2,
        4,5,7,
        7,5,6,
        8,9,11,
        11,9,10,
        12,13,15,
        15,13,14,
        16,17,19,
        19,17,18,
        20,21,23,
        23,21,22
    };

    // Initialize globals
    //
    // gp_model  = load_raw_model(vertices, sizeof(vertices) / sizeof(vertices[0]),
        // texcoords, sizeof(texcoords) / sizeof(texcoords[0]),
        // indices, sizeof(indices) / sizeof(indices[0]));
    gp_model = load_from_obj_file("stall.obj");
    gp_texture = load_texture("stallTexture.png", PNG);
    gp_model_texture = create_model_texture(gp_texture);
    gp_textured_model = create_textured_model(gp_model, gp_model_texture);
    gp_entity = create_entity(gp_textured_model, (vec3) {0.0f, 0.0f, 0.0f}, (vec3) {1.0f, 1.0f, 1.0f}, (vec3) {});
    gp_camera = create_camera(gp_entity);
    gp_shader = create_shader("default.vert", "default.frag");

    // Create projection matrix
    //
    glm_perspective(FOV, get_aspect_ratio(), NEAR_PLANE, FAR_PLANE, g_projection_mat);

    // Load matrices to shader
    //
    bind_shader(gp_shader);
    uniform_mat4(gp_shader, "projection", g_projection_mat);
    unbind_shader();


}

void prepare_frame (void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor (0.0f, 0.3f, 0.7f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
}

void render_model (const Entity * p_entity)
{
    const RawModel * raw_model = (RawModel *) p_entity->p_model;
    glBindVertexArray(raw_model->vao_id);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    bind_shader(gp_shader);
    camera_move(gp_camera);
    mat4 trans = {{0}};
    mat4 view = {{0}};
    get_view_matrix(gp_camera, view);
    increase_rotation(p_entity, (vec3) {0.0f, 0.01f, 0.0f});
    increase_position(p_entity, (vec3) {0.001f, 0.001f, -0.005f});
    create_transformation_matrix(trans, p_entity->position, p_entity->rotation, p_entity->scale);
    uniform_mat4(gp_shader, "view", view);
    uniform_mat4(gp_shader, "transformation", trans);
    bind_texture(gp_texture, 0);
    glDrawElements(GL_TRIANGLES, raw_model->vertex_count, GL_UNSIGNED_INT, 0);
    unbind_shader();
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);
}

void renderer_render_main (void)
{
    prepare_frame();
    render_model(gp_entity);
}

void renderer_cleanup (void)
{
    clean_resources();
    destroy_raw_model(gp_model);
    destroy_textured_model(gp_textured_model);
}