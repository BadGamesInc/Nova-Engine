//
// Created by gregorym on 5/10/26.
//

#ifndef NOVA_ENGINE_FILEUTILS_H
#define NOVA_ENGINE_FILEUTILS_H

char * get_current_running_path (void);
char * get_resource_location (const char * p_resource_path, const char * p_resource_name);
char * load_file_to_string (const char * p_file_path);

#endif //NOVA_ENGINE_FILEUTILS_H
