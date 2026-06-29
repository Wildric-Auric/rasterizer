#ifndef RAS_ASSET_H
#define RAS_ASSET_H
#include <ras_math.h>

struct ras_obj_model_t {
    struct {
    v4f*    pos;
    v3f*    coords;    
    v3f*    normals;    
    } data;

    struct { 
        ui32*   pos;
        ui32*   coord;
        ui32*   normal;
    } indices;


    struct {
        int pos;
        int coord;
        int normal;
    } count;

    int idx_count;
};

struct ras_obj_processed_t {
    v4f*    positions;
    float*  data;
    ui32*   indices;
    int     idx_count;
    int     data_count; 
};

int  ras_load_obj_model(const char* path, ras_obj_model_t* model);
void ras_free_obj_model(ras_obj_model_t* const model);
void ras_make_obj_processed(const ras_obj_model_t* const model, ras_obj_processed_t* out);
void ras_free_obj_processed(ras_obj_processed_t* const);
#endif
