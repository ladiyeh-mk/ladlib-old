#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>

static inline void arena_mem_zero(void *dest, size_t n) { uint8_t *d = dest; while (n > 0) { *d++ = 0; n--; } }

typedef struct {
    max_align_t _alignment;
    uint8_t *buffer;

    size_t offset;
    size_t capacity;
} Arena_t;

Arena_t *arena_new(const size_t capacity) {
    if (!capacity) { return NULL; }

    Arena_t *arena = malloc(sizeof(Arena_t) + capacity);
    if (!arena) { return NULL; }

    arena->buffer   = (uint8_t *) arena + sizeof(Arena_t);
    arena->offset   = 0;
    arena->capacity = capacity;

    return arena;
}

void *arena_alloc_aligned(Arena_t *arena, const size_t size, const size_t align) {
    if (!arena || !size || !align) { return NULL; }
    if ((align & (align - 1)) != 0) { return NULL; }

    size_t padding = (align - (arena->offset & (align - 1))) & (align - 1);

    if (padding > arena->capacity - arena->offset) { return NULL; }
    if (size > arena->capacity - arena->offset - padding) { return NULL; }

    void *ptr     = arena->buffer + arena->offset + padding;
    arena->offset = arena->offset + padding       + size;

    return ptr;
}

void *arena_alloc(Arena_t *arena, const size_t size) {
    return arena_alloc_aligned(arena, size, alignof(max_align_t));
}

bool arena_init(Arena_t *arena, void *buffer, const size_t capacity) {
    if (!arena || !buffer || !capacity) { return false; }

    arena->buffer   = (uint8_t *) buffer;
    arena->offset   = 0;
    arena->capacity = capacity;
    return true;
}

void arena_destroy(Arena_t **arena_ptr) {
    if (!arena_ptr || !*arena_ptr) { return; }

    free(*arena_ptr);
    *arena_ptr = NULL;
}

void arena_reset(Arena_t *arena) {
    if (!arena) { return; }
    arena->offset = 0;
}

void arena_restore(Arena_t *arena, const size_t checkpoint) {
    if (!arena || checkpoint > arena->offset) { return; }
    arena->offset = checkpoint;
}

size_t arena_save(const Arena_t *arena) {
    if (!arena) { return 0; }
    return arena->offset;
}

size_t arena_remaining(const Arena_t *arena) {
    if (!arena) { return 0; }
    return arena->capacity - arena->offset;
}

size_t arena_used(const Arena_t *arena) {
    return arena ? arena->offset : 0;
}