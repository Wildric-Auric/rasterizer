#include "ras_util.h"
#include <stdlib.h>
#include <string.h>

void* ras_alloc_f(int size) {
    return malloc(size);
}

void ras_grow_realloc_f(void** ptr, int* old_cap, int new_cap, int element_size) {
    if (*old_cap >= new_cap) return;
    *ptr     = realloc(*ptr, element_size * (new_cap) * 2);
    *old_cap = new_cap * 2;
}

void ras_realloc_f(void** ptr, int new_size) {
    *ptr = realloc(*ptr, new_size);jjj
}

void ras_free_f(void** ptr) {
    free(*ptr);
    *ptr = 0;
}

void ras_memcpy(void* const dst, const void* const src, int size) {
    memcpy(dst, src, size);
}