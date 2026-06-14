#ifndef RAS_UTIL_H
#define RAS_UTIL_H

#define ras_alloc(type)     (type*)malloc(sizeof(type));
#define ras_alloc_n(type,n) (type*)malloc(sizeof(type)*n);
#define ras_free(ptr)       free(ptr); ptr = 0;

#endif
