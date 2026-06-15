#version 400 core

layout (location = 0) in vec3  position;
layout (location = 1) in vec2  tex_coords;
layout (location = 2) in vec3  normal;
layout (location = 3) in ivec4 bone_indices;
layout (location = 4) in vec4  bone_weights;

const int MAX_BONES = 100;

out vec2  pass_tex_coords;
out vec3  surface_normal;
out vec3  to_light_vector;
out vec3  to_cam_vector;
out float visibility;

uniform mat4  transformation;
uniform mat4  projection;
uniform mat4  view;
uniform vec3  light_position;
uniform bool  use_fake_normals;
uniform float fog_density;
uniform float fog_gradient;
uniform mat4  bone_matrices[MAX_BONES];
uniform bool  is_animated;
uniform float num_rows;
uniform vec2  tex_offset;

void main()
{
    vec4 total_position = vec4(0.0);
    vec3 total_normal   = vec3(0.0);

    if (is_animated)
    {
        for (int i = 0; i < 4; i++)
        {
            if (bone_indices[i] < 0) continue;
            mat4 bone_matrix  = bone_matrices[bone_indices[i]];
            total_position   += bone_matrix * vec4(position, 1.0) * bone_weights[i];
            total_normal     += mat3(bone_matrix) * normal * bone_weights[i];
        }
    }
    else
    {
        total_position = vec4(position, 1.0);
        total_normal   = normal;
    }

    vec4 world_pos      = transformation * total_position;
    vec4 pos_rel_to_cam = view * world_pos;
    vec3 actual_normal  = normal;
    gl_Position         = projection * pos_rel_to_cam;
    pass_tex_coords     = (tex_coords / num_rows) + tex_offset;
    
    if (use_fake_normals)
    {
        total_normal = vec3(0.0, 1.0, 0.0);
    }

    surface_normal  = (transformation * vec4(total_normal, 0.0)).xyz;
    to_light_vector = light_position - world_pos.xyz;
    to_cam_vector   = (inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - world_pos.xyz;

    float dist = length(pos_rel_to_cam.xyz);
    visibility = clamp(exp(-pow((dist * fog_density), fog_gradient)), 0.0, 1.0);
}