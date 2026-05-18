//
// Created by gregorym on 5/11/26.
//

#include <engine.h>
#include <window.h>
#include <stdlib.h>

#include <util/novalogger.h>
#include <util/config.h>

#include <render/model/resourceloader.h>

#include "render/renderer.h"

// Main window handle
//
GLFWwindow * gp_window = NULL;
int32_t      g_window_width = 1280;
int32_t      g_window_height = 720;

/**
 * Initialize everything about the engine
 */
void nova_init()
{
    // Create the logger
    //
    char * p_config_path = get_config_path("logs/");
    nova_init_logger(p_config_path);
    free(p_config_path);

    nova_info("Initializing Nova Engine %d.%d", NOVA_VERSION_MAJOR, NOVA_VERSION_MINOR);

    // Create the game window
    //
    gp_window = create_window(g_window_width, g_window_height, "GAMEEEE");

    nova_info("Initializing Render Engine");
    renderer_init();

    show_window(gp_window);
}

int32_t get_window_width (void)
{
    return g_window_width;
}

int32_t get_window_height (void)
{
    return g_window_height;
}

float get_aspect_ratio (void)
{
    return (float) g_window_width / (float) g_window_height;
}

/**
 * Clean up any used resources and exit gracefully
 */
void stop_engine (void)
{
    renderer_cleanup();

    // Close down logger
    //
    nova_exit_logger();

    // Exit glfw
    //
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

/**
 * Called once per frame, all rendering logic performed here
 */
void render (void)
{
    renderer_render_main();
}

/**
 * Initialize the engine and start the main loop
 */
void run_engine (void)
{
    nova_init();

    while (!glfwWindowShouldClose(gp_window))
    {
        render();

        glfwSwapBuffers(gp_window);
        glfwWaitEvents();
    }

    stop_engine();
}

void on_resize (GLFWwindow * p_window, int32_t width, int32_t height)
{
    g_window_width = width;
    g_window_height = height;
}

void on_key (GLFWwindow * p_window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
{

}

void on_mouse_press (GLFWwindow * p_window, int32_t button, int32_t action, int32_t mods)
{

}

void on_cursor_move (GLFWwindow * p_window, int32_t x, int32_t y)
{

}

void on_scroll (GLFWwindow * p_window, double xoffset, double yoffset)
{

}