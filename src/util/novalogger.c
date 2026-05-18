//
// Created by gregorym on 5/2/26.
//

#include <util/novalogger.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

FILE * gp_log_file;

void nova_init_logger (const char * log_dir)
{
    const time_t      now          = time(0);
    const struct tm * p_time            = localtime(&now);
    char              time_str[80] = {0};
    strftime(time_str, sizeof(time_str), "%Y-%m-%d-%H-%M-%S", p_time);

    // +2: possible missing trailing '/' and null terminator
    const size_t name_len      = strlen(log_dir) + strlen(time_str) + 2;
    char *       log_file_name = malloc(name_len);

    // Ensure log_dir ends with a separator
    if (log_dir[strlen(log_dir) - 1] == '/')
    {
        snprintf(log_file_name, name_len, "%s%s", log_dir, time_str);
    }
    else
    {
        snprintf(log_file_name, name_len, "%s/%s", log_dir, time_str);
    }

    gp_log_file = fopen(log_file_name, "w");
    free(log_file_name);
    if (gp_log_file == NULL)
    {
        printf("Error creating log file\n");
        exit(-1);
    }
}

void nova_log (const int level, const char * format, ...)
{
    va_list args          = {0};
    char    message[1024] = {0};

    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    const time_t      time_now     = time(0);
    const struct tm * p_time       = localtime(&time_now);
    char              time_str[80] = {0};

    strftime(time_str, sizeof(time_str), "[%Y-%m-%d-%H-%M-%S] ", p_time);

    const char * p_level_str;

    switch (level)
    {
        case NOVA_ERROR:   p_level_str = "(Nova) ERROR: ";   break;
        case NOVA_WARNING: p_level_str = "(Nova) WARNING: "; break;
        case NOVA_INFO:
        default:           p_level_str = "(Nova) INFO: ";    break;
    }

    fprintf(gp_log_file, "%s%s%s\n", time_str, p_level_str, message);
    printf("%s%s%s\n", time_str, p_level_str, message);
}

void nova_info (const char * format, ...)
{
    va_list args          = {0};
    char    message[1024] = {0};
    va_start(args, format);

    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    nova_log(NOVA_INFO, "%s", message);
}

void nova_warn (const char * format, ...)
{
    va_list args          = {0};
    char    message[1024] = {0};
    va_start(args, format);

    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    nova_log(NOVA_WARNING, "%s", message);
}

void nova_error (const char * format, ...)
{
    va_list args          = {0};
    char    message[1024] = {0};
    va_start(args, format);

    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    nova_log(NOVA_ERROR, "%s", message);
}

void nova_exit_logger (void)
{
    fclose(gp_log_file);
}