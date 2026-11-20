#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define DEFAULT_ARRAY_CAPACITY 16
static inline void array_mem_copy(void *dest, const void *src, size_t n) { uint8_t *d = dest; const uint8_t *s = src; while (n > 0) { *d++ = *s++; n--; } }

typedef struct {
    max_align_t _alignment;
    size_t item_size, len, capacity;
} ArrayHeader;

void *array_new(size_t item_size) {
    if (!item_size) { return NULL; }
    if (item_size > (SIZE_MAX - sizeof(ArrayHeader)) / DEFAULT_ARRAY_CAPACITY) { return NULL; }

    void *raw = malloc(sizeof(ArrayHeader) + (item_size * DEFAULT_ARRAY_CAPACITY));
    if (!raw) { return NULL; }

    ArrayHeader *header = (ArrayHeader *) raw;
    header->item_size   = item_size;
    header->capacity    = DEFAULT_ARRAY_CAPACITY;
    header->len         = 0;

    return (void *) ((uint8_t *) raw + sizeof(ArrayHeader));
}

void *array_reserve(void **array_ptr, const size_t capacity) {
    if (!array_ptr || !*array_ptr) { return NULL; }
    if (!capacity) { return *array_ptr; }

    void *array         = *array_ptr;
    ArrayHeader *header = (ArrayHeader *) ((uint8_t *) array - sizeof(ArrayHeader));
    
    if (capacity <= header->capacity) { return *array_ptr; }
    if (capacity > (SIZE_MAX - sizeof(ArrayHeader)) / header->item_size) { return NULL; }

    void *raw = realloc(header, sizeof(ArrayHeader) + (capacity * header->item_size));
    if (!raw) { return NULL; }

    header           = (ArrayHeader *) raw;
    header->capacity = capacity;

    *array_ptr = (void *) ((uint8_t *) raw + sizeof(ArrayHeader));

    return *array_ptr;
}

bool array_append(void **array_ptr, void *item, size_t item_size) {
    if (!array_ptr || !*array_ptr || !item || !item_size) { return false; }

    void *array         = *array_ptr;
    ArrayHeader *header = (ArrayHeader *) ((uint8_t *) array - sizeof(ArrayHeader));
    if (header->item_size != item_size) { return false; }

    // i wouldn't need this code is it wasn't for chatgpt scaring me about other user stupidity
    uintptr_t item_addr  = (uintptr_t) item;
    uintptr_t array_addr = (uintptr_t) array;
    bool      item_inside_array = item_addr >= array_addr &&
                                    item_addr <  array_addr + header->len * header->item_size;
    size_t    item_offset       = item_inside_array ? (size_t) (item_addr - array_addr) : 0;

    if (header->len >= header->capacity) {
        size_t new_cap = header->capacity * 2;
        if (!array_reserve(array_ptr, new_cap)) { return false; }
        array  = *array_ptr;
        header = (ArrayHeader *) ((uint8_t *) array - sizeof(ArrayHeader));
    }

    if (item_inside_array) { item = (uint8_t *) array + item_offset; }

    void *destination = (uint8_t *) array + (header->len * header->item_size);
    array_mem_copy(destination, item, item_size);
    header->len++;

    return true;
}

bool array_pop(void *array, void *out_dest) {
    if (!array || !out_dest) { return false; }

    ArrayHeader *header = (ArrayHeader *) ((uint8_t *) array - sizeof(ArrayHeader));
    if (header->len == 0) { return false; }

    header->len--;
    void *src = (uint8_t *) array + (header->len * header->item_size);
    array_mem_copy(out_dest, src, header->item_size);
    return true;
}

bool array_insert(void **array_ptr, const void *item, size_t item_size, size_t index) {
    if (!array_ptr || !*array_ptr || !item) { return false; }

    void *array = *array_ptr;
    ArrayHeader *header = (ArrayHeader *)((uint8_t *)array - sizeof(ArrayHeader));

    if (item_size != header->item_size) { return false; }
    if (index > header->len) { return false; }

    void *item_copy = malloc(item_size);
    if (!item_copy) { return false; }
    array_mem_copy(item_copy, item, item_size);

    if (!array_append(array_ptr, item_copy, item_size)) {
        free(item_copy);
        return false;
    }

    array = *array_ptr;
    header = (ArrayHeader *)((uint8_t *)array - sizeof(ArrayHeader));

    for (size_t i = header->len - 1; i > index; i--) {
        array_mem_copy(
            (uint8_t *)array + (i * header->item_size),
            (uint8_t *)array + ((i - 1) * header->item_size),
            header->item_size
        );
    }

    array_mem_copy((uint8_t *)array + (index * header->item_size), item_copy, header->item_size);

    free(item_copy);
    return true;
}

bool array_remove(void **array_ptr, size_t item_size, size_t index) {
    if (!array_ptr || !*array_ptr) { return false; }

    void *array = *array_ptr;
    ArrayHeader *header = (ArrayHeader *)((uint8_t *)array - sizeof(ArrayHeader));

    if (item_size != header->item_size) { return false; }
    if (index >= header->len) { return false; }

    for (size_t i = index; i < header->len - 1; i++) {
        array_mem_copy(
            (uint8_t *)array + (i * header->item_size),
            (uint8_t *)array + ((i + 1) * header->item_size),
            header->item_size
        );
    }

    header->len--;
    return true;
}

void array_clear(void *array) {
    if (!array) { return; }

    ArrayHeader *header = (ArrayHeader *) ((uint8_t *) array - sizeof(ArrayHeader));
    header->len       = 0;
}

void array_destroy(void **array_ptr) {
    if (!array_ptr || !*array_ptr) { return; }

    void *array = *array_ptr;
    void *raw   = (ArrayHeader *) ((uint8_t *) array - sizeof(ArrayHeader));

    free(raw);
    *array_ptr = NULL;
}

size_t array_count(const void *array) {
    if (!array) { return 0; }

    const ArrayHeader *header = (const ArrayHeader *) ((const uint8_t *) array - sizeof(ArrayHeader));
    return header->len;
}

size_t array_capacity(const void *array) {
    if (!array) { return 0; }

    const ArrayHeader *header = (const ArrayHeader *) ((const uint8_t *) array - sizeof(ArrayHeader));
    return header->capacity;
}