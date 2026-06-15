//
// Created by gregorym on 5/30/26.
//

#include "util/containers.h"
#include "util/nova_logger.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "render/model/model.h"

/* General */

// Default String comparison
//
int container_strcmp (const void * p_key1, const void * p_key2)
{
    return strcmp((const char *)p_key1, (const char *)p_key2);
}

/* ArrayLists */

struct ArrayList
{
    void **     p_array;
    size_t      list_size;
    size_t      capacity;
    CompareFunc compare_func;
};

// Default comparison function
//
int default_compare (const void * p_key1, const void * p_key2)
{
    if (p_key1 == p_key2)
    {
        return 0;
    }

    return 1;
}

/**
 * Creates a new ArrayList pointer
 * @param initial_capacity The initial size to allocate for the ArrayList
 * (initial_capacity * sizeof(void *)
 * @param compare_func The function used to compare values in the ArrayList, use
 * NULL for default pointer1 == pointer2
 * @return Pointer to the new ArrayList
 */
ArrayList * create_arraylist (size_t initial_capacity, CompareFunc compare_func)
{
    // Allocate ArrayList struct
    //
    ArrayList * p_list = malloc(sizeof(*p_list));

    if (p_list == NULL)
    {
        printf("malloc failed\n");
        return NULL;
    }

    // Make sure initial capacity is not zero to avoid undefined behavior
    //
    if (initial_capacity == 0)
    {
        initial_capacity = 1;
    }

    if (compare_func == NULL)
    {
        compare_func = default_compare;
    }

    // Allocate the internal array
    //
    p_list->p_array      = malloc(initial_capacity * sizeof(void *));
    p_list->list_size    = 0;
    p_list->capacity     = initial_capacity;
    p_list->compare_func = compare_func;

    return p_list;
}

/**
 * Free the ArrayList and elements
 * @param p_list The ArrayList to free
 * @param free_func The function to use to free the internal list items, use
 * NULL if they should not be freed here
 */
void arraylist_free (ArrayList * p_list, const FreeFunc free_func)
{
    if (free_func != NULL)
    {
        for (size_t i = 0; i < p_list->list_size; i++)
        {
            free_func(p_list->p_array[i]);
        }
    }

    free(p_list->p_array);
    free(p_list);
}

// Function to grow the internal array
//
void arraylist_resize (ArrayList * p_list)
{
    if (p_list->capacity == 0)
    {
        p_list->capacity = 1;
    }

    // Reallocate the internal array to twice the size
    //
    size_t  new_capacity = p_list->capacity * 2;
    void ** new_array = realloc(p_list->p_array, new_capacity * sizeof(void *));

    if (new_array == NULL)
    {
        nova_error("ArrayList realloc failed");
        return;
    }

    p_list->capacity = new_capacity;
    p_list->p_array  = new_array;
}

/**
 * Adds a value to the ArrayList
 * @param p_list The ArrayList to add to
 * @param p_value The value to add to the ArrayList
 */
void arraylist_add (ArrayList * p_list, void * p_value)
{
    if (p_list->p_array == NULL)
    {
        p_list->p_array = malloc(p_list->capacity * sizeof(void *));
    }

    if ((float)p_list->list_size / (float)p_list->capacity >= 0.75)
    {
        arraylist_resize(p_list);
    }

    p_list->p_array[p_list->list_size] = p_value;
    p_list->list_size++;
}

void arraylist_set (ArrayList * p_list, size_t index, void * p_value)
{
    if (index >= p_list->list_size)
    {
        nova_error("index out of range");
        return;
    }

    p_list->p_array[index] = p_value;
}

/**
 * Adds a heap allocated string value to the array list
 * @param p_list The list to add the string to
 * @param p_value The string to add
 */
void arraylist_add_string (ArrayList * p_list, const char * p_value)
{
    arraylist_add(p_list, strdup(p_value));
}

/**
 * Removes a value from the ArrayList if it contains it
 * @param p_list The ArrayList to remove the value from
 * @param p_value The value to remove
 * @param free_func The function to use to free the resource, NULL to not free
 */
void arraylist_remove (ArrayList * p_list, void * p_value, FreeFunc free_func)
{
    size_t index = arraylist_index_of(p_list, p_value);
    if (index == SIZE_MAX)
    {
        return;
    }

    if (free_func != NULL)
    {
        free_func(p_list->p_array[index]);
    }

    for (size_t i = index; i < p_list->list_size - 1; i++)
    {
        p_list->p_array[i] = p_list->p_array[i + 1];
    }

    p_list->list_size--;
}

/**
 * Retrieves the item at the specified index from the ArrayList
 * @param p_list The ArrayList to get the item from
 * @param index The index of the item to get
 * @return The item at the specified index
 */
void * arraylist_get (const ArrayList * p_list, const size_t index)
{
    if (index >= p_list->list_size)
    {
        printf("index out of range\n");
        return NULL;
    }

    return p_list->p_array[index];
}

/**
 * Returns the index of provided value if it exists in the ArrayList
 * @param p_list The ArrayList to check
 * @param p_value The value to find the index of
 * @return The index of the value if the list contains it, SIZE_MAX if not found
 */
size_t arraylist_index_of (const ArrayList * p_list, const void * p_value)
{
    for (size_t i = 0; i < p_list->list_size; i++)
    {
        if (p_list->compare_func(p_list->p_array[i], p_value) == 0)
        {
            return i;
        }
    }

    return SIZE_MAX;
}

/**
 * Gets the size of the ArrayList
 * @param p_list The ArrayList to get the size of
 * @return The size of the ArrayList
 */
size_t arraylist_count (const ArrayList * p_list)
{
    return p_list->list_size;
}

/**
 * Clear all elements from the ArrayList
 * @param p_list The ArrayList to clear
 * @param free_func The function to free the elements with, use NULL if they
 * should not be freed
 */
void arraylist_clear (ArrayList * p_list, const FreeFunc free_func)
{
    // Free elements if free_func is not NULL
    //
    if (free_func != NULL)
    {
        for (size_t i = 0; i < p_list->list_size; i++)
        {
            if (p_list->p_array[i] != NULL)
            {
                free_func(p_list->p_array[i]);
                p_list->p_array[i] = NULL;
            }
        }
    }

    // Clear the value of all elements in the list
    //
    memset(p_list->p_array, 0, p_list->capacity * sizeof(void *));
    p_list->list_size = 0;
}

/* HashMap */

typedef struct Node
{
    void *        p_key;
    void *        p_value;
    struct Node * next;
} Node;

struct HashMap
{
    Node **     buckets;
    size_t      capacity;
    size_t      size;
    HashFunc    hash;
    CompareFunc compare;
    ArrayList * p_keys;
    ArrayList * p_values;
};

// Compute a hash for the provided string (djb2 algorithm)
//
unsigned int hashmap_str_hash (const void * p_key)
{
    const char * str   = (const char *)p_key;
    uint64_t     value = 5381;

    while (*str)
    {
        value = ((value << 5) + value) + *str++;
    }
    return (unsigned int)value;
}

// Knuth Multiplicative variant hash for integer indices
//
unsigned int int_hash (const void * key)
{
    if (key == NULL)
    {
        nova_error("Key is NULL");
        return 0;
    }

    const int value = *(const int *)key;
    return (unsigned int)value * 2654435761U;
}

unsigned int char_hash (const void * key)
{
    if (key == NULL)
    {
        nova_error("Key is NULL");
        return 0;
    }
    const char value = *(const char *)key;
    return (unsigned int)value * 2654435761U;
}

int container_char_compare (const void * key1, const void * key2)
{
    const char k1 = *(const char *)key1;
    const char k2 = *(const char *)key2;

    if (k1 < k2) return -1;
    if (k1 > k2) return 1;
    return 0;
}

char * char_dup(const char value) {
    char * p_dup = malloc(sizeof(*p_dup));
    if (p_dup == NULL) {
        nova_error("Failed to allocate char_dup");
        return NULL;
    }
    *p_dup = value;
    return p_dup;
}

int container_int_compare (const void * key1, const void * key2)
{
    const int k1 = *(const int *)key1;
    const int k2 = *(const int *)key2;

    if (k1 < k2)
    {
        return -1;
    }

    if (k1 > k2)
    {
        return 1;
    }

    return 0;
}

int container_uint_compare (const void * key1, const void * key2)
{
    const unsigned int k1 = *(const unsigned int *)key1;
    const unsigned int k2 = *(const unsigned int *)key2;

    if (k1 < k2)
    {
        return -1;
    }

    if (k1 > k2)
    {
        return 1;
    }

    return 0;
}

unsigned int hashmap_texture_model_hash (const void * key)
{
    const TexturedModel * p_model = (const TexturedModel *)key;
    unsigned int          hash    = 2166136261U;
    const unsigned int    prime   = 16777619U;

    hash ^= p_model->raw_model.vao_id;
    hash *= prime;
    hash ^= p_model->raw_model.vertex_count;
    hash *= prime;
    hash ^= p_model->texture.texture_id;
    hash *= prime;

    float sd = p_model->texture.shine_damper;
    if (sd == -0.0f)
    {
        sd = 0.0f;
    }

    unsigned int sd_bits;
    memcpy(&sd_bits, &sd, sizeof(unsigned int));
    hash ^= sd_bits;
    hash *= prime;

    float ref = p_model->texture.reflectivity;
    if (ref == -0.0f)
    {
        ref = 0.0f;
    }

    unsigned int ref_bits;
    memcpy(&ref_bits, &ref, sizeof(unsigned int));
    hash ^= ref_bits;
    hash *= prime;

    return hash;
}

int hashmap_texture_model_compare (const void * p_key1, const void * p_key2)
{
    const TexturedModel * p_m1 = (const TexturedModel *)p_key1;
    const TexturedModel * p_m2 = (const TexturedModel *)p_key2;

    if (p_m1->raw_model.vao_id != p_m2->raw_model.vao_id
        || p_m1->raw_model.vertex_count != p_m2->raw_model.vertex_count
        || p_m1->texture.texture_id != p_m2->texture.texture_id)
    {
        return 1;
    }

    const float epsilon = 0.00001f;

    if (fabsf(p_m1->texture.shine_damper - p_m2->texture.shine_damper) > epsilon
        || fabsf(p_m1->texture.reflectivity - p_m2->texture.reflectivity)
               > epsilon)
    {
        return 1;
    }

    return 0;
}

/**
 * Allocates and copies the provided 32 bit integer onto the heap
 * @param value The int to store
 * @return Pointer to the copied value
 */
int * int_dup (int value)
{
    int * p_dup = malloc(sizeof(*p_dup));
    if (p_dup == NULL)
    {
        nova_error("Failed to allocate int_dup");
        return NULL;
    }
    *p_dup = value;
    return p_dup;
}

/**
 * Create a pointer to a new HashMap
 * @param initial_capacity The initial capacity of the HashMap
 * @param hash_func The function to hash the key with
 * @param value_comp_func The function to compare keys with
 * @return The pointer to the new HashMap
 */
HashMap * create_hashmap (size_t            initial_capacity,
                          const HashFunc    hash_func,
                          const CompareFunc key_compare_func,
                          const CompareFunc value_comp_func)
{
    // Allocate the actual HashMap struct
    //
    HashMap * map = malloc(sizeof(*map));

    if (map == NULL)
    {
        nova_error("Failed to create HashMap");
        return NULL;
    }

    if (initial_capacity == 0)
    {
        initial_capacity = 4;
    }

    map->capacity = initial_capacity;
    map->size     = 0;
    map->hash     = hash_func;
    map->compare  = key_compare_func;
    map->buckets  = calloc(initial_capacity, sizeof(*map->buckets));
    map->p_keys   = create_arraylist(initial_capacity, key_compare_func);
    map->p_values = create_arraylist(initial_capacity, value_comp_func);

    if (map->buckets == NULL)
    {
        nova_error("Failed to allocate buckets matrix");
        free(map);
        return NULL;
    }

    return map;
}

// Internal tool to insert items without checking or running resize code
// recursively
//
static void hashmap_put_raw (Node **        target_buckets,
                             const size_t   target_capacity,
                             Node *         p_node,
                             const HashFunc hash_fn)
{
    const unsigned int index = hash_fn(p_node->p_key) % target_capacity;
    p_node->next             = target_buckets[index];
    target_buckets[index]    = p_node;
}

// Resizes the map to a larger capacity securely
//
static void hashmap_resize (HashMap * map)
{
    const size_t old_capacity = map->capacity;
    Node **      old_buckets  = map->buckets;

    const size_t new_capacity = old_capacity * 2;
    Node **      new_buckets  = calloc(new_capacity, sizeof(*new_buckets));

    if (new_buckets == NULL)
    {
        nova_error("Realloc failure during hash table extension");
        return; // Retain old small layout on allocation failure
    }

    for (size_t i = 0; i < old_capacity; i++)
    {
        Node * current = old_buckets[i];
        while (current != NULL)
        {
            Node * next_node = current->next;
            hashmap_put_raw(new_buckets, new_capacity, current, map->hash);
            current = next_node;
        }
    }

    map->capacity = new_capacity;
    map->buckets  = new_buckets;
    free(old_buckets);
}

/**
 * Puts a new key-value pair in the given HashMap
 * @param p_map The HashMap to put the key-value pair into
 * @param p_key The key to put
 * @param p_value The value to put
 */
void hashmap_put (HashMap * p_map, void * p_key, void * p_value)
{
    if ((float)p_map->size / (float)p_map->capacity >= 0.75f)
    {
        hashmap_resize(p_map);
    }

    const unsigned int index   = p_map->hash(p_key) % p_map->capacity;
    Node *             current = p_map->buckets[index];

    while (current != NULL)
    {
        if (p_map->compare(current->p_key, p_key) == 0)
        {
            current->p_value = p_value;

            for (size_t i = 0; i < p_map->p_keys->list_size; i++)
            {
                if (p_map->compare(p_map->p_keys->p_array[i], p_key) == 0)
                {
                    arraylist_set(p_map->p_values, i, p_value);
                    break;
                }
            }
            return;
        }
        current = current->next;
    }

    Node * new_node = malloc(sizeof(*new_node));

    if (new_node == NULL)
    {
        nova_error("Failed to create new_node");
        return;
    }

    new_node->p_key       = p_key;
    new_node->p_value     = p_value;
    new_node->next        = p_map->buckets[index];
    p_map->buckets[index] = new_node;
    p_map->size++;

    arraylist_add(p_map->p_keys, p_key);
    arraylist_add(p_map->p_values, p_value);
}

/**
 * Gets the value from the given key from the provided HashMap
 * @param p_map The HashMap to get the value from
 * @param p_key The key to use to retrieve the value
 * @return The value associated with the key or NULL if not present
 */
void * hashmap_get (const HashMap * p_map, const void * p_key)
{
    const unsigned int index   = p_map->hash(p_key) % p_map->capacity;
    Node *             current = p_map->buckets[index];

    while (current != NULL)
    {
        if (p_map->compare(current->p_key, p_key) == 0)
        {
            return current->p_value;
        }
        current = current->next;
    }

    return NULL;
}

ArrayList * hashmap_keys (const HashMap * p_map)
{
    return p_map->p_keys;
}

ArrayList * hashmap_values (const HashMap * p_map)
{
    return p_map->p_values;
}

size_t hashmap_size (const HashMap * p_map)
{
    return p_map->size;
}

/**
 * Delete a key-value pair from the HashMap
 * @param p_map The HashMap to delete the pair from
 * @param p_key The key to use
 * @param free_key The function to use to free the key
 * @param free_value The function to use to free the value
 * @return 1 if found and deleted, 0 if not found
 */
int hashmap_remove (HashMap *      p_map,
                    const void *   p_key,
                    const FreeFunc free_key,
                    const FreeFunc free_value)
{
    const unsigned int index    = p_map->hash(p_key) % p_map->capacity;
    Node *             current  = p_map->buckets[index];
    Node *             previous = NULL;

    while (current != NULL)
    {
        if (p_map->compare(current->p_key, p_key) == 0)
        {
            if (previous == NULL)
            {
                p_map->buckets[index] = current->next;
            }
            else
            {
                previous->next = current->next;
            }

            arraylist_remove(p_map->p_keys, current->p_key, NULL);
            arraylist_remove(p_map->p_values, current->p_value, NULL);

            if (free_key != NULL)
            {
                free_key(current->p_key);
            }
            if (free_value != NULL)
            {
                free_value(current->p_value);
            }

            free(current);
            p_map->size--;
            return 1;
        }

        previous = current;
        current  = current->next;
    }

    return 0;
}

/**
 * Clears all key-value pairs from the HashMap
 * @param p_map The map to clear
 * @param free_key The method to use to free keys
 * @param free_value The method to use to free values
 */
void hashmap_clear (HashMap * p_map, FreeFunc free_key, FreeFunc free_value)
{
    if (p_map == NULL)
    {
        return;
    }

    for (size_t i = 0; i < p_map->capacity; i++)
    {
        Node * current = p_map->buckets[i];
        while (current != NULL)
        {
            Node * temp = current;
            current     = current->next;

            if (free_key != NULL)
            {
                free_key(temp->p_key);
            }

            if (free_value != NULL)
            {
                free_value(temp->p_value);
            }

            free(temp);
        }
    }

    // Set all values to null
    memset(p_map->buckets, 0, p_map->capacity * sizeof(Node *));

    if (p_map->p_keys != NULL)
    {
        memset(p_map->p_keys->p_array,
               0,
               p_map->p_keys->capacity * sizeof(void *));
        p_map->p_keys->list_size = 0;
    }

    if (p_map->p_values != NULL)
    {
        memset(p_map->p_values->p_array,
               0,
               p_map->p_values->capacity * sizeof(void *));
        p_map->p_values->list_size = 0;
    }

    p_map->size = 0;
}

void free_hashmap (HashMap *      p_map,
                   const FreeFunc free_key,
                   const FreeFunc free_value)
{
    if (p_map == NULL)
    {
        return;
    }

    for (size_t i = 0; i < p_map->capacity; i++)
    {
        Node * current = p_map->buckets[i];
        while (current != NULL)
        {
            Node * temp = current;
            current     = current->next;

            if (free_key != NULL)
            {
                free_key(temp->p_key);
            }

            if (free_value != NULL)
            {
                free_value(temp->p_value);
            }

            free(temp);
        }
    }

    arraylist_free(p_map->p_keys, NULL);
    arraylist_free(p_map->p_values, NULL);

    free(p_map->buckets);
    free(p_map);
}