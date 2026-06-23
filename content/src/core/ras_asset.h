#ifndef RAS_ASSET_H
#define RAS_ASSET_H

struct ras_obj_model_t {
    float*  triangles;    
    int     count;
};

int  ras_load_obj_model(const char* path, ras_obj_model_t* tris);
void ras_free_obj_model(void** data);
#endif
