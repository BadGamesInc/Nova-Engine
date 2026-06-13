//
// Created by gregorym on 5/10/26.
//

#include <render/model/resource_loader.h>
#include <gl.h>
#include <stdlib.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "util/fileutils.h"
#include "util/nova_logger.h"

unsigned int * vao_list      = NULL;
unsigned int * vbo_list      = NULL;
unsigned int * texture_list  = NULL;
unsigned int   vao_count     = 0;
unsigned int   vbo_count     = 0;
unsigned int   texture_count = 0;

void init_resource_loader (void)
{
    vao_list     = (unsigned int *)malloc(sizeof(unsigned int));
    vbo_list     = (unsigned int *)malloc(sizeof(unsigned int));
    texture_list = (unsigned int *)malloc(sizeof(unsigned int));

    if (vao_list == NULL || vbo_list == NULL || texture_list == NULL)
    {
        nova_error("Failed to allocate resource trackers");
    }
}

// Add new VAOs to this list to discard them later
//
void add_vao (const unsigned int vao_id)
{
    unsigned int * new_list
        = realloc(vao_list, sizeof(unsigned int) * (vao_count + 1));

    if (new_list == NULL)
    {
        nova_error("Failed to reallocate VAO tracker");
        return;
    }

    vao_list            = new_list;
    vao_list[vao_count] = vao_id;
    vao_count++;
}

// Add new VBOs to this list to discard them later
//
void add_vbo (const unsigned int vbo_id)
{
    unsigned int * new_list
        = realloc(vbo_list, sizeof(unsigned int) * (vbo_count + 1));

    if (new_list == NULL)
    {
        nova_error("Failed to reallocate VBO tracker");
        return;
    }

    vbo_list            = new_list;
    vbo_list[vbo_count] = vbo_id;
    vbo_count++;
}

// Add new textures to this list ot discard later
//
void add_texture (const unsigned int texture_id)
{
    unsigned int * new_list
        = realloc(texture_list, sizeof(unsigned int) * (texture_count + 1));

    if (new_list == NULL)
    {
        nova_error("Failed to reallocate texture tracker");
        return;
    }

    texture_list                = new_list;
    texture_list[texture_count] = texture_id;
    texture_count++;
}

/**
 * Stores the provided vertices in the provided attribute of the VAO
 * @param attribute The attribute to store the data in
 * @param dimensions The dimension of the coordinates
 * @param p_data The vertices to store in the vao
 * @param data_length The number of vertices in the array
 */
void store_data_vao (const int          attribute,
                     const int          dimensions,
                     const float *      p_data,
                     const unsigned int data_length)
{
    unsigned int vbo_id = 0;
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(data_length * sizeof(float)),
                 p_data,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(attribute, dimensions, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    add_vbo(vbo_id);
}

/**
 * Stores the provided indices in a VBO
 * @param p_indices The indices to store
 * @param indices_length The size of the indices array
 */
void bind_indices_buffer (const unsigned int * p_indices,
                          const unsigned int   indices_length)
{
    unsigned int vbo_id = 0;
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (GLsizeiptr)(indices_length * sizeof(unsigned int)),
                 p_indices,
                 GL_STATIC_DRAW);
    add_vbo(vbo_id);
}

/**
 * Loads a vertex and index array into a raw model for rendering
 * @param p_vertices The array of vertices
 * @param vertices_length The size of the vertex array
 * @param p_texture_coords The array of texture coordinates
 * @param texture_coords_length The size of the texture coords array
 * @param p_indices The array of indices
 * @param indices_length The size of the index array
 * @param p_normals The normals of the mesh
 * @param normals_length The size of the normal array
 * @return A pointer to the newly created RawModel
 */
RawModel * load_raw_model (const float *        p_vertices,
                           const unsigned int   vertices_length,
                           const float *        p_texture_coords,
                           const unsigned int   texture_coords_length,
                           const unsigned int * p_indices,
                           const unsigned int   indices_length,
                           const float *        p_normals,
                           const unsigned int   normals_length)
{
    unsigned int vao_id = 0;
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);
    add_vao(vao_id);
    bind_indices_buffer(p_indices, indices_length);
    store_data_vao(0, 3, p_vertices, vertices_length);
    store_data_vao(1, 2, p_texture_coords, texture_coords_length);
    store_data_vao(2, 3, p_normals, normals_length);
    glBindVertexArray(0);

    RawModel * p_model = create_raw_model(vao_id, (int)indices_length);

    return p_model;
}

/**
 * Loads an OBJ model into a raw model
 * This method uses assimp and assumes the number of meshes is 1
 * @param p_obj_filename The name of the obj file
 */
RawModel * load_from_obj_file (const char * p_obj_filename)
{
    const char * p_obj_filepath
        = get_resource_location("models", p_obj_filename);
    const struct aiScene * scene
        = aiImportFile(p_obj_filepath,
                       aiProcess_CalcTangentSpace | aiProcess_Triangulate
                           | aiProcess_JoinIdenticalVertices
                           | aiProcess_SortByPType | aiProcess_FlipUVs);

    if (scene == NULL)
    {
        nova_error("Failed to load model %s", p_obj_filepath);
        nova_error("%s", aiGetErrorString());
        return NULL;
    }

    // Get the aiMesh struct and extract all the relevant data
    //
    const struct aiMesh * p_mesh            = scene->mMeshes[0];
    const unsigned int    vertices_length   = p_mesh->mNumVertices * 3;
    const unsigned int    tex_coords_length = p_mesh->mNumVertices * 2;
    unsigned int          indices_length    = 0;
    const float *         p_vertices        = (float *)p_mesh->mVertices;
    float *            p_tex_coords = malloc(sizeof(float) * tex_coords_length);
    unsigned int *     p_indices    = NULL;
    const float *      p_normals    = (float *)p_mesh->mNormals;
    const unsigned int normals_length = p_mesh->mNumVertices * 3;

    if (p_tex_coords == NULL)
    {
        nova_error("Failed to allocate texture coordinate data");
        aiReleaseImport(scene);
        return NULL;
    }

    for (unsigned int i = 0; i < p_mesh->mNumVertices; i++)
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
    p_indices = (unsigned int *)malloc(sizeof(unsigned int) * indices_length);

    if (p_indices == NULL)
    {
        nova_error("Failed to allocate index data");
        free(p_tex_coords);
        aiReleaseImport(scene);
        return NULL;
    }

    // Store indices data
    //
    int index_counter = 0;
    for (int i = 0; i < p_mesh->mNumFaces; i++)
    {
        const struct aiFace p_face = p_mesh->mFaces[i];
        for (int j = 0; j < p_face.mNumIndices; j++)
        {
            p_indices[index_counter++] = p_face.mIndices[j];
        }
    }

    // Create RawModel with data from OBJ
    //
    RawModel * p_model = load_raw_model(p_vertices,
                                        vertices_length,
                                        p_tex_coords,
                                        tex_coords_length,
                                        p_indices,
                                        indices_length,
                                        p_normals,
                                        normals_length);

    // Free resources used
    free(p_tex_coords);
    free(p_indices);
    aiReleaseImport(scene);

    return p_model;
}

Texture * load_texture (const char * texture_name)
{
    Texture * p_texture = create_texture(texture_name);
    add_texture(p_texture->texture_id);

    return p_texture;
}

static Texture * load_texture_path (const char * p_absolute_path)
{
    Texture * p_texture = create_texture_from_path(p_absolute_path);
    if (p_texture != NULL)
    {
        add_texture(p_texture->texture_id);
    }
    return p_texture;
}

// Load an assimp mesh into a RawModel
static RawModel * load_ai_mesh (const struct aiMesh * p_mesh)
{
    const unsigned int vertex_count      = p_mesh->mNumVertices;
    const unsigned int vertices_length   = vertex_count * 3;
    const unsigned int tex_coords_length = vertex_count * 2;
    const unsigned int normals_length    = vertex_count * 3;

    // Tex coords
    float * p_tex_coords = malloc(sizeof(float) * tex_coords_length);
    if (p_tex_coords == NULL)
    {
        nova_error("Failed to allocate texture coords for mesh %s", p_mesh->mName.data);
        return NULL;
    }

    for (unsigned int i = 0; i < vertex_count; i++)
    {
        if (p_mesh->mTextureCoords[0] != NULL)
        {
            p_tex_coords[i * 2 + 0] = p_mesh->mTextureCoords[0][i].x;
            p_tex_coords[i * 2 + 1] = p_mesh->mTextureCoords[0][i].y;
        }
        else
        {
            p_tex_coords[i * 2 + 0] = 0.0f;
            p_tex_coords[i * 2 + 1] = 0.0f;
        }
    }

    // Indices
    unsigned int indices_length = 0;
    for (unsigned int i = 0; i < p_mesh->mNumFaces; i++)
    {
        indices_length += p_mesh->mFaces[i].mNumIndices;
    }

    unsigned int * p_indices = malloc(sizeof(unsigned int) * indices_length);
    if (p_indices == NULL)
    {
        nova_error("Failed to allocate indices for mesh %s", p_mesh->mName.data);
        free(p_tex_coords);
        return NULL;
    }

    unsigned int idx = 0;
    for (unsigned int i = 0; i < p_mesh->mNumFaces; i++)
    {
        const struct aiFace face = p_mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            p_indices[idx++] = face.mIndices[j];
        }
    }

    RawModel * p_model = load_raw_model(
        (const float *)p_mesh->mVertices,   vertices_length,
        p_tex_coords,                        tex_coords_length,
        p_indices,                           indices_length,
        (const float *)p_mesh->mNormals,    normals_length
    );

    free(p_tex_coords);
    free(p_indices);

    return p_model;
}

static char * build_texture_path (const struct aiScene * p_scene,
                                  const unsigned int     material_index,
                                  const char *           model_dir)
{
    const struct aiMaterial * p_mat = p_scene->mMaterials[material_index];
    struct aiString           tex_path;

    if (aiGetMaterialTexture(p_mat, aiTextureType_DIFFUSE, 0,
                             &tex_path, NULL, NULL, NULL, NULL, NULL, NULL)
        != AI_SUCCESS)
    {
        return NULL; // no diffuse texture on this material (e.g., Eye_Crystal)
    }

    // Normalize backslashes from Windows-authored MTLs
    for (char * c = tex_path.data; *c != '\0'; c++)
    {
        if (*c == '\\') *c = '/';
    }

    // model_dir already ends with '/' so concatenate directly:
    // e.g. ".../Stylized_Paladin/" + "Textures/Armor_Base_color.png"
    const size_t len = strlen(model_dir) + strlen(tex_path.data) + 1;
    char * p_full_path = malloc(len);
    if (p_full_path == NULL) return NULL;
    snprintf(p_full_path, len, "%s%s", model_dir, tex_path.data);

    return p_full_path;
}

LoadedModel * load_model_from_obj (const char * p_model_folder, const char * p_obj_filename, const unsigned int asset_id)
{
    char p_folder_key[256];
    snprintf(p_folder_key, sizeof(p_folder_key), "models/%s", p_model_folder);

    const char * p_obj_filepath = get_resource_location(p_folder_key, p_obj_filename);
    const char * p_model_dir    = get_resource_location(p_folder_key, "");

    const struct aiScene * p_scene
        = aiImportFile(p_obj_filepath,
                   aiProcess_CalcTangentSpace
                       | aiProcess_Triangulate
                       | aiProcess_JoinIdenticalVertices
                       | aiProcess_SortByPType
                       | aiProcess_FlipUVs);

    if (p_scene == NULL)
    {
        nova_error("Failed to load model %s", p_obj_filepath);
        nova_error("%s", aiGetErrorString());
        return NULL;
    }

    const unsigned int mesh_count = p_scene->mNumMeshes;

    // Allocate model
    LoadedModel * p_model = malloc(sizeof(*p_model));

    if (p_model == NULL)
    {
        nova_error("Failed to allocate LoadedModel");
        aiReleaseImport(p_scene);
        return NULL;
    }

    p_model->p_meshes   = malloc(sizeof(*p_model->p_meshes) * mesh_count);
    p_model->mesh_count = mesh_count;
    p_model->asset_id   = malloc(sizeof(unsigned int));
    *p_model->asset_id  = asset_id;

    if (p_model->p_meshes == NULL)
    {
        nova_error("Failed to allocate LoadedModel meshes");
        free(p_model);
        aiReleaseImport(p_scene);
        return NULL;
    }

    Texture ** pp_texture_cache
        = calloc(p_scene->mNumMaterials, sizeof(Texture *));

    if (pp_texture_cache == NULL)
    {
        nova_error("Failed to allocate LoadedModel texture cache");
        free(p_model->p_meshes);
        free(p_model);
        aiReleaseImport(p_scene);
        return NULL;
    }

    // Load each mesh
    for (unsigned int i = 0; i < mesh_count; i++)
    {
        const struct aiMesh * p_ai_mesh = p_scene->mMeshes[i];
        MeshEntry * p_mesh              = &p_model->p_meshes[i];

        p_mesh->p_raw_model = load_ai_mesh(p_ai_mesh);
        if (p_mesh->p_raw_model == NULL)
        {
            nova_error("Failed to load mesh %u %s", i, p_ai_mesh->mName.data);
            p_mesh->p_texture = NULL;
            continue;
        }

        const unsigned int mat_index = p_ai_mesh->mMaterialIndex;
        if (pp_texture_cache[mat_index] == NULL)
        {
            char * tex_filename = build_texture_path(p_scene, mat_index, p_model_dir);
            if (tex_filename != NULL)
            {
                pp_texture_cache[mat_index] = load_texture_path(tex_filename);
                free(tex_filename);
            }
        }
        p_mesh->p_texture = pp_texture_cache[mat_index];

        float shininess = 0.0f;
        float opacity   = 1.0f;
        const struct aiMaterial * p_mat = p_scene->mMaterials[p_ai_mesh->mMaterialIndex];
        aiGetMaterialFloat(p_mat, AI_MATKEY_SHININESS, &shininess);
        aiGetMaterialFloat(p_mat, AI_MATKEY_OPACITY, &opacity);
        struct aiColor4D specular;
        if (aiGetMaterialColor(p_mat, AI_MATKEY_COLOR_SPECULAR, &specular) == AI_SUCCESS)
        {
            p_mesh->reflectivity = (specular.r + specular.g + specular.b) / 3.0f;
            p_mesh->shine_damper = glm_max(shininess, 10.0f);
            // p_mesh->reflectivity = 2.0f;
            nova_info("%f %f", p_mesh->reflectivity, p_mesh->shine_damper);
        }
        else
        {
            p_mesh->reflectivity = 0.0f;
            p_mesh->shine_damper = 1.0f;
        }

        p_mesh->has_transparency = opacity < 1.0f;
        p_mesh->use_fake_normals = false;
    }

    free(pp_texture_cache);
    aiReleaseImport(p_scene);

    nova_info("Loaded model %s with %u meshes", p_obj_filepath, mesh_count);
    return p_model;
}

void destroy_loaded_model(void * p_value)
{
    LoadedModel * p_loaded_model = p_value;
    if (p_loaded_model == NULL)
    {
        return;
    }

    for (unsigned int i = 0; i < p_loaded_model->mesh_count; i++)
    {
        free(p_loaded_model->p_meshes[i].p_raw_model);
    }

    free(p_loaded_model->p_meshes);
    free(p_loaded_model);
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

    free(vao_list);
    free(vbo_list);
    free(texture_list);

    vao_list      = NULL;
    vbo_list      = NULL;
    texture_list  = NULL;
    vao_count     = 0;
    vbo_count     = 0;
    texture_count = 0;
}