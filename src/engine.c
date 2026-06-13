//
// Created by gregorym on 5/11/26.
//

#include <engine.h>
#include <stdlib.h>

#include "window.h"
#include "util/nova_logger.h"
#include "util/config.h"
#include "util/input.h"
#include "render/asset_manager.h"
#include "render/model/resource_loader.h"
#include "render/renderer.h"
#include "entity/light.h"
#include "util/containers.h"
#include "time.h"
#include "terrain/terrain.h"

// Main window handle
//
GLFWwindow *                gp_window               = NULL;
int                         g_window_width          = 1280;
int                         g_window_height         = 720;
static double               fps                     = 0.0f;
static Camera *             gp_camera               = NULL;
 Entity *             gp_entity               = NULL;
static Light *              gp_light                = NULL;
static ArrayList *          gp_entities             = NULL;
static ArrayList *          gp_terrains             = NULL;
static TerrainTexturePack * gp_terrain_texture_pack = NULL;
static Texture *            gp_blend_map            = NULL;
static Image *              gp_heightmap            = NULL;

/**
 * Initialize everything about the engine
 */
void nova_init ()
{
    // Create the logger
    //
    char * p_config_path = get_config_path("logs/");
    nova_init_logger(p_config_path);
    free(p_config_path);

    nova_info("Initializing Nova Engine %d.%d",
              NOVA_VERSION_MAJOR,
              NOVA_VERSION_MINOR);

    // Create the game window
    //
    gp_window = create_window(g_window_width, g_window_height, "GAMEEEE");

    // Load all game assets
    //
    nova_info("Loading assets");
    init_asset_manager();

    gp_entity = create_entity(gp_women,
                              (vec3) { 0.0f, 0.0f, 0.0f },
                              (vec3) { 1.0f, 1.0f, 1.0f },
                              (vec3) { 0.0f, -3.0f, -20.0f },
                              0);
    gp_camera = create_camera(gp_entity);
    gp_light  = create_light((vec3) { 3000.0f, 2000.0f, 20.0f },
                             (vec3) { 1.0f, 1.0f, 1.0f });

    gp_entities = create_arraylist(256, STRUCT_COMPARE_FUNC(Entity, id));

    srand((unsigned int)glfwGetTime());

    for (int i = 0; i < 1000; i++)
    {
        float x     = ((float)rand() / RAND_MAX) * 1600.0f - 800.0f;
        float y     = 0.0f;
        float z     = ((float)rand() / RAND_MAX) * -800.0f;
        float scale = ((float)rand() / RAND_MAX) * 1.0f + 0.1f;

        arraylist_add(gp_entities,
                      create_entity(gp_fern,
                                    (vec3) { 0.0f, 0.0f, 0.0f },
                                    (vec3) { scale, scale, scale },
                                    (vec3) { x, y, z },
                                    i + 1));

        x     = ((float)rand() / RAND_MAX) * 1600.0f - 800.0f;
        z     = ((float)rand() / RAND_MAX) * -800.0f;
        scale = ((float)rand() / RAND_MAX) * 1.0f + 0.1f;

        arraylist_add(gp_entities,
                      create_entity(gp_grass_fan,
                                    (vec3) { 0.0f, 0.0f, 0.0f },
                                    (vec3) { scale, scale, scale },
                                    (vec3) { x, y, z },
                                    i + 1));

        x     = ((float)rand() / RAND_MAX) * 1600.0f - 800.0f;
        z     = ((float)rand() / RAND_MAX) * -800.0f;
        scale = ((float)rand() / RAND_MAX) * 1.0f + 0.1f;

        arraylist_add(gp_entities,
                      create_entity(gp_low_poly_tree,
                                    (vec3) { 0.0f, 0.0f, 0.0f },
                                    (vec3) { scale, scale, scale },
                                    (vec3) { x, y, z },
                                    i + 1));
    }

    gp_terrains = create_arraylist(16, STRUCT_COMPARE_FUNC(Terrain, id));
    gp_terrain_texture_pack = create_terrain_texture_pack(load_texture("grassy2.png"),
        load_texture("mud.png"),
        load_texture("grassFlowers.png"),
        load_texture("path.png"));
    gp_blend_map = load_texture("blendMap.png");
    gp_heightmap = load_image("heightmap.png");
    arraylist_add(gp_terrains, create_terrain(0, -1, gp_heightmap, gp_terrain_texture_pack, gp_blend_map));
    arraylist_add(gp_terrains, create_terrain(1, -1, gp_heightmap, gp_terrain_texture_pack, gp_blend_map));
    arraylist_add(gp_terrains, create_terrain(-1, -1, gp_heightmap, gp_terrain_texture_pack, gp_blend_map));


    nova_info("Initializing Render Engine");
    renderer_init();

    show_window(gp_window);
}

/**
 * Gets the window handle
 * @return The handle of the game window
 */
GLFWwindow * get_window_handle (void)
{
    return gp_window;
}

int get_window_width (void)
{
    return g_window_width;
}

int get_window_height (void)
{
    return g_window_height;
}

float get_aspect_ratio (void)
{
    return (float)g_window_width / (float)g_window_height;
}

double get_fps (void)
{
    return fps;
}

/**
 * Clean up any used resources and exit gracefully
 */
void stop_engine (void)
{
    input_cleanup();

    // Free assets
    //
    clean_assets();
    clean_resources();

    // Free all arrays containing allocated objects
    //
    arraylist_free(gp_entities, destroy_entity);
    arraylist_free(gp_terrains, destroy_terrain);

    destroy_camera(gp_camera);
    destroy_light(gp_light);
    destroy_image(gp_heightmap);
    destroy_terrain_texture_pack(gp_terrain_texture_pack);
    destroy_texture(gp_blend_map);

    // Cleanup engine systems
    //
    renderer_cleanup();
    nova_exit_logger();

    // Exit glfw
    //
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

/**
 * Handles all non-rendering logic in the game
 */
void tick(void)
{
    if (is_key_pressed(GLFW_KEY_W))
    {
        glm_vec3_add(gp_entity->position, (vec3) { 0.0f, 0.0f, -1.0f }, gp_entity->position);
    }
    else if (is_key_pressed(GLFW_KEY_S))
    {
        glm_vec3_add(gp_entity->position, (vec3) { 0.0f, 0.0f, 1.0f }, gp_entity->position);
    }

    if (is_key_pressed(GLFW_KEY_A))
    {
        glm_vec3_add(gp_entity->position, (vec3) { -1.0f, 0.0f, 0.0f }, gp_entity->position);
    }
    else if (is_key_pressed(GLFW_KEY_D))
    {
        glm_vec3_add(gp_entity->position, (vec3) { 1.0f, 0.0f, 0.0f }, gp_entity->position);
    }

    if (is_key_pressed(GLFW_KEY_SPACE))
    {
        glm_vec3_add(gp_entity->position, (vec3) { 0.0f, 1.0f, 0.0f }, gp_entity->position);
    }
    else if (is_key_pressed(GLFW_KEY_LEFT_SHIFT))
    {
        glm_vec3_add(gp_entity->position, (vec3) { 0.0f, -1.0f, 0.0f }, gp_entity->position);
    }

    camera_move(gp_camera);
}

/**
 * Called once per frame, all rendering logic performed here
 */
void render (void)
{
    for (int i = 0; i < arraylist_count(gp_entities); i++)
    {
        Entity * p_entity = arraylist_get(gp_entities, i);
        process_entity(p_entity);
    }

    for (int i = 0; i < arraylist_count(gp_terrains); i++)
    {
        process_terrain(arraylist_get(gp_terrains, i));
    }

    process_entity(gp_entity);
    renderer_render_main(gp_camera, gp_light);
}

/**
 * Initialize the engine and start the main loop
 */
void run_engine (void)
{
    nova_init();

    double last_time   = glfwGetTime();
    int    frame_count = 0;
    double fps_timer   = 0.0;

    while (!glfwWindowShouldClose(gp_window))
    {
        const double current_time = glfwGetTime();
        const double delta_time   = current_time - last_time;
        last_time                 = current_time;

        fps_timer += delta_time;
        frame_count++;

        if (fps_timer >= 1.0)
        {
            fps         = frame_count / fps_timer;
            frame_count = 0;
            fps_timer   = 0.0;
        }

        char title_buffer[128];
        sprintf(title_buffer,
                "Nova Engine %d.%d | %lf FPS",
                NOVA_VERSION_MAJOR,
                NOVA_VERSION_MINOR,
                fps);
        glfwSetWindowTitle(gp_window, title_buffer);

        tick();
        render();

        glfwSwapBuffers(gp_window);
        glfwPollEvents();
    }

    stop_engine();
}

void on_resize (GLFWwindow * p_window, const int width, const int height)
{
    g_window_width  = width;
    g_window_height = height;
    glViewport(0, 0, width, height);
}

void on_key (GLFWwindow * p_window, int key, int scancode, int action, int mods)
{
}

void on_mouse_press (GLFWwindow * p_window, int button, int action, int mods)
{
}

void on_cursor_move (GLFWwindow * p_window, int x, int y)
{
    update_cursor_pos();
}

void on_scroll (GLFWwindow * p_window, double xoffset, double yoffset)
{
    update_mouse_scroll(xoffset, yoffset);
}