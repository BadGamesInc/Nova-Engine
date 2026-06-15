//
// Created by gregorym on 6/7/26.
//

#include "util/input.h"

#include <stdlib.h>

#include "engine.h"
#include "util/nova_logger.h"

static double * gp_mouse_x       = NULL;
static double * gp_mouse_y       = NULL;
static double   g_mouse_scroll_x = 0.0;
static double   g_mouse_scroll_y = 0.0;

bool is_key_pressed (const int key)
{
    return glfwGetKey(get_window_handle(), key) == GLFW_PRESS;
}

bool is_mouse_button_pressed (const int button)
{
    return glfwGetMouseButton(get_window_handle(), button) == GLFW_PRESS;
}

// Update the current cursor positions
//
void update_cursor_pos (void)
{
    if (gp_mouse_x == NULL || gp_mouse_y == NULL)
    {
        gp_mouse_x = malloc(sizeof(double));

        if (gp_mouse_x == NULL)
        {
            nova_error("Failed to allocate mouse x");
            return;
        }

        gp_mouse_y = malloc(sizeof(double));

        if (gp_mouse_y == NULL)
        {
            nova_error("Failed to allocate mouse x");
            return;
        }
    }

    glfwGetCursorPos(get_window_handle(), gp_mouse_x, gp_mouse_y);
}

void update_mouse_scroll (double xoffset, double yoffset)
{
    g_mouse_scroll_x = xoffset;
    g_mouse_scroll_y = yoffset;
}

// Return the x coordinate of the cursor
//
double get_mouse_x (void)
{
    if (gp_mouse_x == NULL)
    {
        update_cursor_pos();
    }

    return *gp_mouse_x;
}

// Return the y coordinate of the cursor
//
double get_mouse_y (void)
{
    if (gp_mouse_y == NULL)
    {
        update_cursor_pos();
    }

    return *gp_mouse_y;
}

double get_mouse_scroll_x (void)
{
    return g_mouse_scroll_x;
}

double get_mouse_scroll_y (void)
{
    return g_mouse_scroll_y;
}

// Free allocated memory for mouse positions
//
void input_cleanup (void)
{
    free(gp_mouse_x);
    free(gp_mouse_y);
}