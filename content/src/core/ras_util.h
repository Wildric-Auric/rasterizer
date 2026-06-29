#ifndef RAS_UTIL_H
#define RAS_UTIL_H

#define ras_alloc(type)     (type*)ras_alloc_f(sizeof(type));
#define ras_alloc_n(type,n) (type*)ras_alloc_f(sizeof(type)*n);
#define ras_free(ptr)              ras_free_f((void**)&ptr);
#define ras_grow_realloc_n(ptr_out, type, cmp_cap_ptr, new_cap) \
    ras_grow_realloc_f((void**)(ptr_out), (cmp_cap_ptr), (new_cap), sizeof(type));
#define ras_realloc(ptr, type, size) ras_realloc_f((void**)&(ptr), sizeof(type) * (size))


void* ras_alloc_f(const int size);
void ras_realloc_f(void** ptr, const int new_size);
void ras_grow_realloc_f(void** ptr, int* old_cap, int new_cap, const int element_size);
void ras_free_f(void** ptr);
void ras_memcpy(void* const dst, const void* const src, const int size);

#endif
