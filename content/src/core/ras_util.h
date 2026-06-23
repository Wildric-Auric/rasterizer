#ifndef RAS_UTIL_H
#define RAS_UTIL_H

#define ras_alloc(type)     (type*)malloc(sizeof(type));
#define ras_alloc_n(type,n) (type*)malloc(sizeof(type)*n);
#define ras_free(ptr)       free(ptr); ptr = 0;
#define ras_grow_realloc_n(ptr, type, cmp_cap, new_cap) \
    if (cmp_cap < new_cap) { \
        ptr = (type*)realloc((ptr), sizeof(type) * (new_cap) * 2);\
        cmp_cap = new_cap * 2; \
    }

#endif
