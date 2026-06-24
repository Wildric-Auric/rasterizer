#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ras_asset.h"
#include "ras_math.h"
#include "ras_util.h"

enum model_parse_state_ {
    model_parse_state_general,
    model_parse_state_comment,
    model_parse_state_face,
    model_parse_state_face_fan,
    model_parse_state_vertex,
};

bool is_blank(char c) {
    return c == '\t' || c == ' ' || c == '\n' ;
}

void skip_blanks(FILE* f) {
    char c;
    int x;
    while ((x = fread(&c, 1, 1, f))) {
        if (c != '\t' && c != ' ') 
            break;
    }
    if (x)
        fseek(f, -1, SEEK_CUR); 
}

int parse_word(FILE* f, char* out, bool* endl, int* r) {
    char c;
    int  idx = 0;
    out[0] = 0;
    skip_blanks(f);
    while (1) {
        *r = fread(&c, 1, 1, f);
        if (is_blank(c) || !*r) break;
        out[idx] = c;
        ++idx;
    }
    *endl = c == '\n';
    if (!*endl) fseek(f, -1, SEEK_CUR);
    out[idx] = 0;
    return idx;
}

int ras_load_obj_model(const char* path, ras_obj_model_t* out_data) {
    FILE*  f;
    int  r = 1;
    char   c;
    char wrd[64];
    bool endl;
    int vert_cap = 1024;
    int idx_cap  = 1024; 
    model_parse_state_ state = model_parse_state_general;
    v4f vertex;
    out_data->vertices = ras_alloc_n(v4f, vert_cap);
    out_data->indices  = ras_alloc_n(ui32, idx_cap);
    out_data->vert_count = 0;
    out_data->idx_count  = 0;
    v3i32  tri;
    vertex.w = 1.0;
    f = fopen(path, "r");
    if (!f) { printf("Error opening file at: %s", path); return 1;}
    while (r) {
        endl = 0;
        wrd[0] = 0;
        switch (state) {
            case model_parse_state_general: {
                parse_word(f, wrd, &endl, &r); 
                if (wrd[0] == '#') 
                    state = model_parse_state_comment;
                if (!strcmp("f", wrd)) {
                    state = model_parse_state_face;
                    tri = v3i32(-1,-1,-1);
                    break;
                }
                if (!strcmp("v", wrd)) {
                    state = model_parse_state_vertex;
                    break; 
                }
                break;
            }
            case model_parse_state_comment: {
                parse_word(f, wrd, &endl, &r);
                if (endl) state = model_parse_state_general;
                break;
            }
            case model_parse_state_face: {
                parse_word(f, wrd, &endl, &r);
                tri.x = atoi(wrd) - 1;
                parse_word(f, wrd, &endl, &r);
                tri.y = atoi(wrd) - 1;
                parse_word(f, wrd, &endl, &r);
                tri.z = atoi(wrd) - 1;
                ras_grow_realloc_n(&out_data->indices, ui32, &idx_cap, out_data->idx_count+3+1); 
                out_data->indices[out_data->idx_count+0] = tri.x;
                out_data->indices[out_data->idx_count+1] = tri.y;
                out_data->indices[out_data->idx_count+2] = tri.z;
                tri.y = tri.z;
                out_data->idx_count += 3;
                if (endl)
                    state = model_parse_state_general;    
                else
                    state = model_parse_state_face_fan;
                break;
            }
            case model_parse_state_face_fan: {
                parse_word(f, wrd, &endl, &r);
                if (!wrd[0]) {
                    state = model_parse_state_general;
                    break;
                }
                tri.z = atoi(wrd) - 1;
                ras_grow_realloc_n(&out_data->indices, ui32, &idx_cap, out_data->idx_count+3+1);
                out_data->indices[out_data->idx_count]   = tri.x;
                out_data->indices[out_data->idx_count+1] = tri.y;
                out_data->indices[out_data->idx_count+2] = tri.z;
                tri.y = tri.z;
                out_data->idx_count += 3;
                if (endl)
                    state = model_parse_state_general;
                break;
            }
            case model_parse_state_vertex: {
                parse_word(f, wrd, &endl, &r);
                vertex.x = atof(wrd);
                parse_word(f, wrd, &endl, &r);
                vertex.y = atof(wrd);
                parse_word(f, wrd, &endl, &r);
                vertex.z = atof(wrd); 
                ras_grow_realloc_n(&out_data->vertices, v4f, &vert_cap, out_data->vert_count+1);
                out_data->vertices[out_data->vert_count] = vertex;
                state = model_parse_state_general;
                out_data->vert_count++;
                break;
            }
            default: break;
        }
    }
    ras_realloc(out_data->indices, ui32, out_data->idx_count);
    ras_realloc(out_data->vertices, v4f, out_data->vert_count);
    fclose(f);
    return 0;
}

void ras_free_obj_model(ras_obj_model_t* const model) {
    ras_free(model->vertices);
    ras_free(model->indices);
    model->vert_count = 0;
    model->idx_count  = 0;
}
