#ifndef RAS_ASSET_H
#define RAS_ASSET_H
#include <ras_math.h>

struct ras_obj_model_t {
    v4f*    vertices;
    ui32*   indices;
    int     idx_count;
    int     vert_count;
};

int  ras_load_obj_model(const char* path, ras_obj_model_t* model);
void ras_free_obj_model(ras_obj_model_t* const model);
#endif
