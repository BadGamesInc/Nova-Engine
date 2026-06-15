#version 400 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoord;

out vec4 vertex_color;
out vec2 frag_tex_coord;

uniform mat4 projection_matrix;
uniform mat4 transformation_matrix;

void main()
{
    gl_Position    = projection_matrix * transformation_matrix * vec4(position, 0.0, 1.0);
    vertex_color   = color;
    frag_tex_coord = texCoord;
}