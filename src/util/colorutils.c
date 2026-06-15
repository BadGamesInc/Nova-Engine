//
// Created by gregorym on 6/9/26.
//

#include "util/colorutils.h"

int get_rgb (const vec3 color)
{
    const int r = (int)color[0];
    const int g = (int)color[1];
    const int b = (int)color[2];
    const int a = 0xFF;

    return (a << 24) | (r << 16) | (g << 8) | b;
}

int get_rgba (const vec4 color)
{
    const int r = (int)color[0];
    const int g = (int)color[1];
    const int b = (int)color[2];
    const int a = (int)color[3];

    return (a << 24) | (r << 16) | (g << 8) | b;
}