//
// Created by gregorym on 5/10/26.
//

#include <render/model/resourceloader.h>
#include <gl.h>
#include <stdlib.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "util/fileutils.h"
#include "util/novalogger.h"

uint32_t * vao_list      = NULL;
uint32_t * vbo_list      = NULL;
uint32_t * texture_list  = NULL;
uint32_t   vao_count     = 0;
uint32_t   vbo_count     = 0;
uint32_t   texture_count = 0;

void init_resource_loader (void)
{
    vao_list     = (uint32_t *) malloc(sizeof(uint32_t));
    vbo_list     = (uint32_t *) malloc(sizeof(uint32_t));
    texture_list = (uint32_t *) malloc(sizeof(uint32_t));

    if (vao_list == NULL || vbo_list == NULL || texture_list == NULL)
    {
        nova_error("Failed to allocate resource trackers");
    }
}

// Add new VAOs to this list to discard of them later
//
void add_vao (const uint32_t vao_id)
{
    vao_list = realloc(vao_list, sizeof(uint32_t) * (vao_count + 1));

    if (vao_list == NULL)
    {
        nova_error("Failed to reallocate VAO tracker");
        free(vao_list);
        return;
    }

    vao_list[vao_count] = vao_id;
    vao_count++;
}

// Add new VBOs to this list to discard of them later
//
void add_vbo (const uint32_t vbo_id)
{
    vbo_list = realloc(vbo_list, sizeof(uint32_t) * (vbo_count + 1));

    if (vbo_list == NULL)
    {
        nova_error("Failed to reallocate VBO tracker");
        free(vbo_list);
        return;
    }

    vbo_list[vbo_count] = vbo_id;
    vbo_count++;
}

// Add new textures to this list ot discard later
//
void add_texture (const uint32_t texture_id)
{
    texture_list = realloc(texture_list, sizeof(uint32_t) * (texture_count + 1));

    if (texture_list == NULL)
    {
        nova_error("Failed to reallocate texture tracker");
        free(texture_list);
        return;
    }

    texture_list[texture_count] = texture_id;
    texture_count++;
}

/**
 * Stores the provided vertices in the provided attribute of the VAO
 * @param attribute The attribute to store the data in
 * @param dimensions The dimension of the coordinates
 * @param p_vertices The vertices to store in the vao
 * @param vertices_length The amount of vertices in the array
 */
void store_data_vao(const int32_t attribute, const int32_t dimensions, const float * p_vertices, const int32_t vertices_length)
{
    uint32_t vbo_id = 0;
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (vertices_length * sizeof(float)), p_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(attribute, dimensions, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    add_vbo(vbo_id);
}

/**
 * Stores the provided indices in a VBO
 * @param p_indices The indices to store
 * @param indices_length The size of the indices array
 */
void bind_indices_buffer(const uint32_t * p_indices, const int32_t indices_length)
{
    uint32_t vbo_id = 0;
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr) (indices_length * sizeof(unsigned int)), p_indices, GL_STATIC_DRAW);
    add_vbo(vbo_id);
}

/**
 * Loads a vertex and index array into a raw model for rendering
 * @param vertices The array of vertices
 * @param vertices_length The size of the vertex array
 * @param texture_coords The array of texture coordinates
 * @param texture_coords_length The size of the texture coords array
 * @param indices The array of indices
 * @param indices_length The size of the index array
 * @return A pointer to the newly created RawModel
 */
RawModel * load_raw_model (const float * vertices, const int32_t vertices_length,
                        const float * texture_coords, const int32_t texture_coords_length,
                        const uint32_t * indices, const int32_t indices_length
                        )
{
    uint32_t vao_id = 0;
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);
    add_vao(vao_id);
    bind_indices_buffer(indices, indices_length);
    store_data_vao(0, 3, vertices, vertices_length);
    store_data_vao(1, 2, texture_coords, texture_coords_length);
    glBindVertexArray(0);

    RawModel * p_model = create_raw_model(vao_id, indices_length);

    return p_model;
}

/**
 * Loads an OBJ model into a raw model
 * This method uses assimp and assumes number of meshes is 1
 * @param p_obj_filename The name of the obj file
 */
RawModel * load_from_obj_file(const char * p_obj_filename)
{
    const char * p_obj_filepath = get_resource_location("models", p_obj_filename);
    const struct aiScene * scene = aiImportFile(p_obj_filepath,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate             |
        aiProcess_JoinIdenticalVertices   |
        aiProcess_SortByPType             |
        aiProcess_FlipUVs);

    if (scene == NULL)
    {
        nova_error("Failed to load model %s", p_obj_filepath);
        nova_error("%s", aiGetErrorString());
        return NULL;
    }

    // Get the aiMesh struct and extract all the relevant data
    //
    const struct aiMesh *       p_mesh            = scene->mMeshes[0];
    const uint32_t              vertices_length   = p_mesh->mNumVertices * 3;
    const uint32_t              tex_coords_length = p_mesh->mNumVertices * 2;
    uint32_t                    indices_length    = 0;
    const float *               p_vertices        = (float *) p_mesh->mVertices;
    float *                     p_tex_coords      = malloc(sizeof(float) * tex_coords_length);
    uint32_t *                  p_indices         = NULL;

    if (p_tex_coords == NULL) {
        nova_error("Failed to allocate texture coordinate data");
        aiReleaseImport(scene);
        return NULL;
    }

    for (uint32_t i = 0; i < p_mesh->mNumVertices; i++)
    {
        // Safe check for Channel 0 in pure C
        if (p_mesh->mTextureCoords[0] != NULL)
        {
            // Pull only X and Y, skipping Assimp's internal Z component
            p_tex_coords[i * 2 + 0] = p_mesh->mTextureCoords[0][i].x;
            p_tex_coords[i * 2 + 1] = p_mesh->mTextureCoords[0][i].y;
        }
        else
        {
            // Safe fallback zeros if the mesh features no UV mappings
            p_tex_coords[i * 2 + 0] = 0.0f;
            p_tex_coords[i * 2 + 1] = 0.0f;
        }
    }

    // We must calculate the indices based on the faces
    //
    for (int i = 0; i < p_mesh->mNumFaces; i++)
    {
        indices_length += p_mesh->mFaces[i].mNumIndices;
    }

    // Allocate indices buffer
    //
    p_indices = (uint32_t *) malloc(sizeof(uint32_t) * indices_length);

    if (p_indices == NULL) {
        nova_error("Failed to allocate index data");
        free(p_tex_coords);
        aiReleaseImport(scene);
        return NULL;
    }

    // Store indices data
    //
    int32_t index_counter = 0;
    for (int i = 0; i < p_mesh->mNumFaces; i++)
    {
        struct aiFace p_face = p_mesh->mFaces[i];
        for (int j = 0; j < p_face.mNumIndices; j++)
        {
            p_indices[index_counter++] = p_face.mIndices[j];
        }
    }

    // Create RawModel with data from OBJ
    //
    RawModel * p_model = load_raw_model(p_vertices, vertices_length,
        p_tex_coords, tex_coords_length,
        p_indices, indices_length);

    // Free resources used
    free(p_tex_coords);
    free(p_indices);
    aiReleaseImport(scene);

    return p_model;
}

Texture * load_texture (const char * texture_name, const enum TextureType texture_type)
{
    Texture * p_texture = create_texture(texture_name, texture_type);
    add_texture(p_texture->texture_id);

    return p_texture;
}

// Free any resources allocated
//
void clean_resources (void)
{
    for (int i = 0; i < vao_count; i++)
    {
        glDeleteVertexArrays(1, &vao_list[i]);
    }

    for (int i = 0; i < vbo_count; i++)
    {
        glDeleteBuffers(1, &vbo_list[i]);
    }

    for (int i = 0; i < texture_count; i++)
    {
        glDeleteTextures(1, &texture_list[i]);
    }
}