//
// Created by gregorym on 5/2/26.
//

#ifndef NOVA_ENGINE_NOVALOGGER_H
#define NOVA_ENGINE_NOVALOGGER_H

#define NOVA_ERROR   0
#define NOVA_WARNING 1
#define NOVA_INFO    2
#define NOVA_DEBUG   3

void nova_init_logger (const char *log_dir);
void nova_log (int level, const char *format, ...);
void nova_info (const char *format, ...);
void nova_warn (const char *format, ...);
void nova_error (const char *format, ...);
void nova_exit_logger (void);

#endif //NOVA_ENGINE_NOVALOGGER_H
