#ifndef __LADLIB_ARENA_H
#define __LADLIB_ARENA_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>

typedef struct {
    max_align_t _alignment;
    uint8_t *buffer;

    size_t offset;
    size_t capacity;
} Arena_t;

Arena_t *arena_new(const size_t capacity);
bool arena_init(Arena_t *arena, void *buffer, const size_t capacity);

void *arena_alloc(Arena_t *arena, const size_t size);
void *arena_alloc_aligned(Arena_t *arena, const size_t size, const size_t align);

void arena_reset(Arena_t *arena);
void arena_restore(Arena_t *arena, const size_t checkpoint);
void arena_destroy(Arena_t **arena_ptr);

size_t arena_save(const Arena_t *arena);
size_t arena_used(const Arena_t *arena);
size_t arena_remaining(const Arena_t *arena);

#endif