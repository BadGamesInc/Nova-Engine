//
// Created by gregorym on 5/10/26.
//

#include <util/fileutils.h>
#include <util/novalogger.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <unistd.h>

/**
 * Gets the current path the binary is running from
 * @return The current running path
 */
char * get_current_running_path (void)
{
    static char  path[256]   = {0};
    const size_t path_length = readlink("/proc/self/exe", path, 256);

    if (path_length != -1)
    {
        path[path_length] = '\0';
        dirname(path);
    }
    else
    {
        nova_error("Failed to find current directory");
    }

    return path;
}

/**
 * Gets the path to the specified resource file
 * @param p_resource_path The path of the resource relative to the current running directory
 * @param p_resource_name The name of the resource to get
 * @return The path to the resource
 */
char * get_resource_location (const char * p_resource_path, const char * p_resource_name)
{
    const char * p_resource_str       = "/resources/";
    const char * p_running_path       = get_current_running_path();
    const size_t size                 = strlen(p_running_path) + strlen(p_resource_str) + strlen(p_resource_path) + strlen(p_resource_name) + 2;
    char *       p_full_resource_path = malloc(size);

    snprintf(p_full_resource_path, size, "%s%s%s%s%s", p_running_path, p_resource_str, p_resource_path, "/", p_resource_name);

    return p_full_resource_path;
}

/**
 * Loads all the contents of a file to a string
 * @param p_file_path The path to the file to be loaded
 * @return A string containing the whole contents of the file
 */
char * load_file_to_string (const char * p_file_path)
{
    FILE * p_file = fopen(p_file_path, "r");

    if (p_file == NULL)
    {
        nova_error("Failed to load file %s", p_file_path);
        return NULL;
    }

    char * buffer = NULL;
    long   length = 0;

    fseek(p_file, 0, SEEK_END);
    length = ftell(p_file);
    fseek(p_file, 0 , SEEK_SET);
    buffer = malloc(length + 1);

    if (buffer == NULL)
    {
        nova_error("Failed to allocate buffer for file");
        fclose(p_file);
        return NULL;
    }

    fread(buffer, 1, length, p_file);
    buffer[length] = '\0';
    fclose(p_file);

    return buffer;
}