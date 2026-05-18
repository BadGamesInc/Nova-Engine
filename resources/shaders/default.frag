#version 400 core

in vec3 color;
in vec2 pass_tex_coords;

out vec4 out_color;

uniform sampler2D texture_sampler;

void main()
{
    out_color = texture(texture_sampler, pass_tex_coords);
}