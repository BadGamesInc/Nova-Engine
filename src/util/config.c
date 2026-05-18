//
// Created by gregorym on 5/3/26.
//

#include <util/config.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

/**
 * Recursively create a directory
 * @param p_path The path of the directory to create
 * @return 0 if successful, -1 if not
 */
int mkdir_p (const char * p_path)
{
    char   tmp[256] = {0};
    char * p        = NULL;

    snprintf(tmp, sizeof(tmp), "%s", p_path);
    const size_t len = strlen(tmp);
    if (tmp[len - 1] == '/')
    {
        tmp[len - 1] = 0;
    }

    for (p = tmp + 1; *p; p++)
    {
        if (*p == '/')
        {
            *p = 0;
            // Create the directory; ignore error if it already exists
            if (mkdir(tmp, S_IRWXU) != 0 && errno != EEXIST)
            {
                return -1;
            }
            *p = '/';
        }
    }
    // Create the final leaf directory
    if (mkdir(tmp, S_IRWXU) != 0 && errno != EEXIST)
    {
        return -1;
    }

    return 0;
}

char * get_config_path (const char * p_path)
{
    const char * p_home      = getenv("HOME");
    const char * p_nova_path = "/.nova/";
    char *       p_full_path = malloc(strlen(p_home) + strlen(p_nova_path) + strlen(p_path) + 1);

    sprintf(p_full_path, "%s%s%s", p_home, p_nova_path, p_path);

    struct stat st = {0};

    if (stat(p_full_path, &st) == -1)
    {
        if (mkdir_p(p_full_path) != 0)
        {
            printf("Failed to create directory %s\n", p_full_path);
        }
    }

    return p_full_path;
}

void init_configs()
{

}

void cleanup_configs()
{

}