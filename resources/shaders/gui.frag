#version 400 core

in vec4 vertex_color;
in vec2 frag_tex_coord;

out vec4 out_Color;

uniform sampler2D texture_sampler;
uniform bool      use_texture;
uniform bool      use_color;

void main()
{
    vec4 color = vec4(1.0);

    if (use_texture)
    {
        color = texture(texture_sampler, frag_tex_coord);
    }
    if (use_color)
    {
        color *= vertex_color;
    }

    out_Color = color;
}