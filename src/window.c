//
// Created by gregorym on 5/11/26.
//

#define GLAD_GL_IMPLEMENTATION
#include <gl.h>
#define GLFW_INCLUDE_NONE
#include <stdio.h>
#include <stdlib.h>
#include <util/novalogger.h>
#include <window.h>
#include <engine.h>

void error_callback (int error, const char * description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void keyboard_callback (GLFWwindow * window, int key, int scancode, int action, int mods)
{
    on_key(window, key, scancode, action, mods);
}

void mouse_button_callback (GLFWwindow * window, int button, int action, int mods)
{
    on_mouse_press(window, button, action, mods);
}

void cursor_callback (GLFWwindow * window, double xpos, double ypos)
{
    on_cursor_move(window, xpos, ypos);
}

void scroll_callback (GLFWwindow * window, double xoffset, double yoffset)
{
    on_scroll(window, xoffset, yoffset);
}

void window_size_callback (GLFWwindow * window, int width, int height)
{
    on_resize(window, width, height);
}

GLFWwindow * create_window (const int32_t width, const int32_t height, const char * p_title)
{
    glfwSetErrorCallback(error_callback);
    glfwInitHint(GLFW_PLATFORM, GLFW_ANY_PLATFORM);

    nova_info("Initializing GLFW %d.%d", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR);
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    GLFWwindow * p_window = glfwCreateWindow(width, height, p_title, NULL, NULL);
    if (!p_window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(p_window);

    gladLoadGL(glfwGetProcAddress);

    int gl_major = 0;
    int gl_minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &gl_major);
    glGetIntegerv(GL_MINOR_VERSION, &gl_minor);
    nova_info("Loaded OpenGL %d.%d", gl_major, gl_minor);

    glfwSwapInterval(1);

    glfwSetKeyCallback(p_window, keyboard_callback);
    glfwSetMouseButtonCallback(p_window, mouse_button_callback);
    glfwSetCursorPosCallback(p_window, cursor_callback);
    glfwSetScrollCallback(p_window, scroll_callback);
    glfwSetWindowSizeCallback(p_window, window_size_callback);

    return p_window;
}

void show_window (GLFWwindow * p_window)
{
    glfwShowWindow(p_window);
}