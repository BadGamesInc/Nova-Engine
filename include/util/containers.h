//
// Created by gregorym on 5/30/26.
//

#ifndef NOVA_ENGINE_CONTAINERS_H
#define NOVA_ENGINE_CONTAINERS_H

#include <stddef.h>
#include <stdint.h>

#define STRUCT_COMPARE_FUNC(StructType, FieldName) \
    _Generic((((StructType *)0)->FieldName),       \
        char : container_char_compare,             \
        char *: container_strcmp,                  \
        const char *: container_strcmp,            \
        unsigned int : container_int_compare,      \
        default: container_uint_compare)

// General
//
typedef int (*CompareFunc)(const void * p_key1, const void * p_key2);
typedef void (*FreeFunc)(void * p_key);
int   container_strcmp(const void * p_key1, const void * p_key2);
int   container_int_compare(const void * key1, const void * key2);
int * int_dup(int value);

// ArrayLists
//
typedef struct ArrayList ArrayList;
ArrayList * create_arraylist(size_t initial_capacity, CompareFunc compare_func);
void        arraylist_add(ArrayList * p_list, void * p_value);
void        arraylist_add_string(ArrayList * p_list, const char * p_value);
void   arraylist_remove(ArrayList * p_list, void * p_value, FreeFunc free_func);
void * arraylist_get(const ArrayList * p_list, size_t index);
void   arraylist_set(ArrayList * p_list, size_t index, void * p_value);
size_t arraylist_index_of(const ArrayList * p_list, const void * p_value);
size_t arraylist_count(const ArrayList * p_list);
void   arraylist_clear(ArrayList * p_list, FreeFunc free_func);
void   arraylist_free(ArrayList * p_list, FreeFunc free_func);

// HashMaps
//
typedef struct HashMap HashMap;
typedef unsigned int (*HashFunc)(const void * p_key);
HashMap *    create_hashmap(size_t      initial_capacity,
                            HashFunc    hash_func,
                            CompareFunc key_comp_func,
                            CompareFunc value_comp_func);
unsigned int hashmap_str_hash(const void * p_key);
unsigned int int_hash(const void * key);
unsigned int char_hash(const void * key);
int          container_char_compare(const void * p_key1, const void * p_key2);
int          container_uint_compare (const void * key1, const void * key2);
char *       char_dup(char p_value);
unsigned int hashmap_texture_model_hash(const void * key);
int    hashmap_texture_model_compare(const void * p_key1, const void * p_key2);
void   hashmap_put(HashMap * p_map, void * p_key, void * p_value);
void * hashmap_get(const HashMap * p_map, const void * p_key);
ArrayList * hashmap_keys(const HashMap * p_map);
ArrayList * hashmap_values(const HashMap * p_map);
size_t      hashmap_size(const HashMap * p_map);
int         hashmap_remove(HashMap *    p_map,
                           const void * p_key,
                           FreeFunc     free_key,
                           FreeFunc     free_value);
void hashmap_clear(HashMap * p_map, FreeFunc free_key, FreeFunc free_value);
void free_hashmap(HashMap * p_map, FreeFunc free_key, FreeFunc free_value);

#endif // NOVA_ENGINE_CONTAINERS_H
