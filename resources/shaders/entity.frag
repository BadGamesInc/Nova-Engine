#version 400 core

in vec2  pass_tex_coords;
in vec3  surface_normal;
in vec3  to_light_vector;
in vec3  to_cam_vector;
in float visibility;

out vec4 out_color;

uniform sampler2D model_texture;
uniform vec3      light_color;
uniform float     shine_damper;
uniform float     reflectivity;
uniform vec3      sky_color;

void main()
{
    vec3  unit_normal         = normalize(surface_normal);
    vec3  unit_light_vector   = normalize(to_light_vector);
    float n_dot1              = dot(unit_normal, unit_light_vector);
    float brightness          = max(n_dot1, 0.075);
    vec3  diffuse             = brightness * light_color;
    vec3  unit_vector_to_cam  = normalize(to_cam_vector);
    vec3  light_direction     = -unit_light_vector;
    vec3  reflected_light_dir = reflect(light_direction, unit_normal);
    float specular_factor     = max(dot(reflected_light_dir, unit_vector_to_cam), 0.0);
    float damped_factor       = pow(specular_factor, shine_damper);
    vec3  final_specular      = damped_factor * reflectivity * light_color;
    vec4  texture_color       = texture(model_texture, pass_tex_coords);

    if (texture_color.a < 0.5)
    {
        discard;
    }

    out_color = mix(vec4(sky_color, 1.0), vec4(diffuse, 1.0) * texture_color + vec4(final_specular, 1.0), visibility);
}