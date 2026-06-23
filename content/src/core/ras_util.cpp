#include "ras_util.h"
#include <stdlib.h>

void* ras_alloc_f(int size) {
    return malloc(size);
}

void ras_grow_realloc_f(void** ptr, int* old_cap, int new_cap, int element_size) {
    if (*old_cap >= new_cap) return;
    *ptr     = realloc(*ptr, element_size * (new_cap) * 2);
    *old_cap = new_cap * 2;
}

void ras_free_f(void** ptr) {
    free(*ptr);
    *ptr = 0;
}
