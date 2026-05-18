#version 400 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coords;

out vec3 color;
out vec2 pass_tex_coords;

uniform mat4 transformation;
uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * transformation * vec4(position.x, position.y, position.z, 1.0);
    color = vec3(position.x + 0.5, 1.0, position.y + 0.5);
    pass_tex_coords = tex_coords;
}