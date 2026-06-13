#version 400 core

in vec2  pass_tex_coords;
in vec3  surface_normal;
in vec3  to_light_vector;
in vec3  to_cam_vector;
in float visibility;

out vec4 out_color;

uniform sampler2D background_texture;
uniform sampler2D r_texture;
uniform sampler2D g_texture;
uniform sampler2D b_texture;
uniform sampler2D blend_map;
uniform vec3      light_color;
uniform float     shine_damper;
uniform float     reflectivity;
uniform vec3      sky_color;

void main()
{
    vec4  blend_map_color      = texture(blend_map, pass_tex_coords);
    float back_texture_factor  = 1 - (blend_map_color.r + blend_map_color.g + blend_map_color.b);
    vec2  tiled_coords         = pass_tex_coords * 40.0;
    vec4  background_tex_color = texture(background_texture, tiled_coords) * back_texture_factor;
    vec4  r_tex_color          = texture(r_texture, tiled_coords) * blend_map_color.r;
    vec4  g_tex_color          = texture(g_texture, tiled_coords) * blend_map_color.g;
    vec4  b_tex_color          = texture(b_texture, tiled_coords) * blend_map_color.b;
    vec4  total_color          = background_tex_color + r_tex_color + g_tex_color + b_tex_color;
    vec3  unit_normal          = normalize(surface_normal);
    vec3  unit_light_vector    = normalize(to_light_vector);
    float n_dot1               = dot(unit_normal, unit_light_vector);
    float brightness           = max(n_dot1, 0.075);
    vec3  diffuse              = brightness * light_color;
    vec3  unit_vector_to_cam   = normalize(to_cam_vector);
    vec3  light_direction      = -unit_light_vector;
    vec3  reflected_light_dir  = reflect(light_direction, unit_normal);
    float specular_factor      = max(dot(reflected_light_dir, unit_vector_to_cam), 0.0);
    float damped_factor        = pow(specular_factor, shine_damper);
    vec3  final_specular       = damped_factor * reflectivity * light_color;

    out_color = mix(vec4(sky_color, 1.0), vec4(diffuse, 1.0) * total_color + vec4(final_specular, 1.0), visibility);
}