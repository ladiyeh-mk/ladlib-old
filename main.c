#include <ladlib/array.h>
#include <ladlib/arena.h>

#include <stdio.h>

int main(void) {
    int *nums = array_new(sizeof(int));
    int x = 42;
    array_append((void **)&nums, &x, sizeof(int));
    
    printf("Count: %zu, Value: %d\n", array_count(nums), nums[0]);
    
    array_destroy((void **)&nums);

    int fsize      = 5;
    Arena_t *arena = arena_new(128);
    float *fnums   = arena_alloc(arena, sizeof(float) * fsize);

    for (int i = 0; i < fsize; i++) { fnums[i] = i; }
    for (int i = 0; i < fsize; i++) { printf("fnums[%d]: %.2f\n", i, fnums[i]); }

    arena_destroy(&arena);
    return 0;
}