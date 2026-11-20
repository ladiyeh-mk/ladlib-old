#ifndef __LADLIB_ARRAY_H
#define __LADLIB_ARRAY_H

#include <stddef.h>
#include <stdbool.h>

void *array_new(size_t item_size);
void *array_reserve(void **array_ptr, const size_t capacity);

bool array_append(void **array_ptr, void *item, size_t item_size);
bool array_pop(void *array, void *out_dest);
bool array_insert(void **array_ptr, const void *item, size_t item_size, size_t index);
bool array_remove(void **array_ptr, size_t item_size, size_t index);

void array_clear(void *array);
void array_destroy(void **array_ptr);

size_t array_count(const void *array);
size_t array_capacity(const void *array);

#endif