#version 400 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 tex_coords;
layout (location = 2) in vec3 normal;

out vec2  pass_tex_coords;
out vec3  surface_normal;
out vec3  to_light_vector;
out vec3  to_cam_vector;
out float visibility;

uniform mat4  transformation;
uniform mat4  projection;
uniform mat4  view;
uniform vec3  light_position;
uniform float fog_density;
uniform float fog_gradient;

void main()
{
    vec4 world_pos      = transformation * vec4(position.x, position.y, position.z, 1.0);
    vec4 pos_rel_to_cam = view * world_pos;
    gl_Position         = projection * pos_rel_to_cam;
    pass_tex_coords     = tex_coords;

    surface_normal  = (transformation * vec4(normal, 0.0)).xyz;
    to_light_vector = light_position - world_pos.xyz;
    to_cam_vector   = (inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - world_pos.xyz;

    float dist = length(pos_rel_to_cam.xyz);
    visibility = clamp(exp(-pow((dist * fog_density), fog_gradient)), 0.0, 1.0);
}