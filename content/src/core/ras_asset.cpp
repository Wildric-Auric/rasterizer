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
    model_parse_state_coords,
    model_parse_state_normals
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

struct ras_obj_face_t {
    union {
        ui32 data[3];
        struct {
            ui32 pos_idx;
            ui32 coord_idx;
            ui32 normal_idx;
        };
    };
};

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

int parse_face_element(FILE* f, char* out, bool* endl, int* r, ras_obj_face_t* face) {
    char c;
    int idx = 0;
    char* tmp = out;
    int   cur = 0;
    out[0] = 0;
    *face = {0};
    skip_blanks(f);
    while (1) {
        *r = fread(&c, 1, 1, f);
        if (is_blank(c) || !*r) break;
        if (c == '/') {
            out[idx] = 0;
            face->data[cur] = atoi(tmp);
            tmp = out + idx + 1;
            ++cur;
        }
        out[idx] = c;
        ++idx;
    }
    out[idx] = 0;
    face->data[cur] = atoi(tmp);
    *endl = c == '\n';
    if (!*endl) fseek(f, -1, SEEK_CUR);
    return idx;
}

int ras_load_obj_model(const char* path, ras_obj_model_t* out_data) {
    FILE*  f;
    int  r = 1;
    char   c;
    char wrd[64];
    bool endl;
    int vert_cap   = 1024;
    int idx_cap    = 1024; 
    int tex_cap    = 1024;
    int normal_cap = 1024;
    model_parse_state_ state = model_parse_state_general;
    v4f vertex;
    out_data->data.pos       = ras_alloc_n(v4f, vert_cap);
    out_data->data.coords    = ras_alloc_n(v3f, tex_cap);
    out_data->data.normals   = ras_alloc_n(v3f, normal_cap);
    out_data->indices.pos    = ras_alloc_n(ui32, idx_cap);
    out_data->indices.coord  = ras_alloc_n(ui32, idx_cap);
    out_data->indices.normal = ras_alloc_n(ui32, idx_cap);
    *out_data->data.pos     = v4f();
    *out_data->data.coords  = v3f();
    *out_data->data.normals = v3f();
    out_data->count.pos    = 1;
    out_data->count.coord  = 1;
    out_data->count.normal = 1;
    out_data->idx_count   = 0;
    struct {
        v3i32 pos;
        v3i32 coord;
        v3i32 normal;
    } tri;
    v3i32  crd;
    v3i32  nrm;
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
                    tri.pos    = v3i32(0,0,0); tri.coord  = v3i32(0,0,0); tri.normal = v3i32(0,0,0);
                    break;
                }
                if (!strcmp("v", wrd)) {
                    state = model_parse_state_vertex;
                    break; 
                }
                if (!strcmp("vt", wrd)) {
                    state = model_parse_state_coords;
                    break;
                }
                if (!strcmp("vn", wrd)) {
                    state = model_parse_state_normals;
                    break;
                }
                break;
            }
            case model_parse_state_comment: {
                parse_word(f, wrd, &endl, &r);
                if (endl)
                    state = model_parse_state_general;
                break;
            }
            case model_parse_state_coords: {
                parse_word(f, wrd, &endl, &r);
                int id  = out_data->count.coord;
                ras_grow_realloc_n(&out_data->data.coords, v3f, &tex_cap, id + 1);
                out_data->data.coords[id].x = atof(wrd);
                out_data->count.coord++;
                if (endl) { state = model_parse_state_general; break; }
                parse_word(f, wrd, &endl, &r);
                if (!wrd[0]) { state = model_parse_state_general; break; }
                out_data->data.coords[id].y = atof(wrd);
                if (endl) {state = model_parse_state_general; break;}
                parse_word(f, wrd, &endl, &r);
                out_data->data.coords[id].z = atof(wrd);
                state = model_parse_state_general;
                break;
            }
            case model_parse_state_normals: {
                parse_word(f, wrd, &endl, &r);
                int id = out_data->count.normal;
                ras_grow_realloc_n(&out_data->data.normals, v3f, &normal_cap, id + 1);
                out_data->data.normals[id].x = atof(wrd);
                parse_word(f, wrd, &endl, &r);
                out_data->data.normals[id].y = atof(wrd);
                parse_word(f, wrd, &endl, &r);
                out_data->data.normals[id].z = atof(wrd);
                out_data->count.normal++;
                if (!endl) {
                    parse_word(f, wrd, &endl, &r);
                    if (!endl) {
                        printf("Error during obj parsing of %s\n Incorrect normals bloc", path);
                    }
                }
                state = model_parse_state_general;
                break;
            }
            case model_parse_state_face: {
                #define consume_elem(vert) tri.pos.vert = face.pos_idx; tri.coord.vert = face.coord_idx;\
                tri.normal.vert = face.normal_idx; face = {};
                ras_obj_face_t face;
                parse_face_element(f, wrd, &endl, &r, &face);
                consume_elem(x);
                parse_face_element(f, wrd, &endl, &r, &face);
                consume_elem(y);
                parse_face_element(f, wrd, &endl, &r, &face);
                consume_elem(z);
                const int old_cap = idx_cap;
                ras_grow_realloc_n(&out_data->indices.pos,    ui32, &idx_cap, out_data->idx_count+3+1); idx_cap = old_cap;
                ras_grow_realloc_n(&out_data->indices.normal, ui32, &idx_cap, out_data->idx_count+3+1); idx_cap = old_cap;
                ras_grow_realloc_n(&out_data->indices.coord,  ui32, &idx_cap, out_data->idx_count+3+1); 
                ras_memcpy(out_data->indices.pos    + out_data->idx_count, &tri.pos.x,    sizeof(ui32) * 3);
                ras_memcpy(out_data->indices.coord  + out_data->idx_count, &tri.coord.x,  sizeof(ui32) * 3);
                ras_memcpy(out_data->indices.normal + out_data->idx_count, &tri.normal.x, sizeof(ui32) * 3);
                tri.pos.y    = tri.pos.z;
                tri.coord.y  = tri.coord.z;
                tri.normal.y = tri.normal.z;
                out_data->idx_count += 3;
                if (endl)
                    state = model_parse_state_general;    
                else
                    state = model_parse_state_face_fan;
                break;
            }
            case model_parse_state_face_fan: {
                ras_obj_face_t face;
                parse_face_element(f, wrd, &endl, &r, &face);
                if (!wrd[0]) {
                    state = model_parse_state_general;
                    break;
                }
                consume_elem(z);
                const int old_cap = idx_cap;
                ras_grow_realloc_n(&out_data->indices.pos, ui32, &idx_cap, out_data->idx_count+3+1);    idx_cap = old_cap;
                ras_grow_realloc_n(&out_data->indices.normal, ui32, &idx_cap, out_data->idx_count+3+1); idx_cap = old_cap;
                ras_grow_realloc_n(&out_data->indices.coord,  ui32, &idx_cap, out_data->idx_count+3+1); 
                ras_memcpy(out_data->indices.pos    + out_data->idx_count, &tri.pos.x,    sizeof(ui32) * 3);
                ras_memcpy(out_data->indices.coord  + out_data->idx_count, &tri.coord.x,  sizeof(ui32) * 3);
                ras_memcpy(out_data->indices.normal + out_data->idx_count, &tri.normal.x, sizeof(ui32) * 3);
                tri.pos.y    = tri.pos.z;
                tri.coord.y  = tri.coord.z;
                tri.normal.y = tri.normal.z;
                out_data->idx_count += 3;
                if (endl)
                    state = model_parse_state_general;
                break;
                #undef consume_elem
            }
            case model_parse_state_vertex: {
                parse_word(f, wrd, &endl, &r);
                vertex.x = atof(wrd);
                parse_word(f, wrd, &endl, &r);
                vertex.y = atof(wrd);
                parse_word(f, wrd, &endl, &r);
                vertex.z = atof(wrd); 
                ras_grow_realloc_n(&out_data->data.pos, v4f, &vert_cap, out_data->count.pos+1);
                out_data->data.pos[out_data->count.pos] = vertex;
                state = model_parse_state_general;
                out_data->count.pos++;
                break;
            }
            default: break;
        }
    }
    ras_realloc(out_data->indices.pos, ui32, out_data->idx_count);
    ras_realloc(out_data->indices.coord, ui32, out_data->idx_count);
    ras_realloc(out_data->indices.normal, ui32, out_data->idx_count);

    ras_realloc(out_data->data.pos, v4f, out_data->count.pos);
    ras_realloc(out_data->data.coords, v3f, out_data->count.coord);
    ras_realloc(out_data->data.normals, v3f, out_data->count.normal);
    fclose(f);
    return 0;
}

void ras_free_obj_model(ras_obj_model_t* const model) {
    ras_free(model->data.pos);
    ras_free(model->data.coords);
    ras_free(model->data.normals);
    model->count = {};
}

struct ras_obj_key_t {
    v4f position;
    v3f coord;
    v3f normal;
};

int find_obj_key(ras_obj_key_t* obj, ras_obj_key_t* key, int size) {
    for (int i = 0; i < size; ++i) {
        if (   obj[i].coord == key->coord 
            && obj[i].position == key->position 
            && obj[i].normal == key->normal)
            return i;
    }
    return -1;
}

int find_or_insert_obj_key(ras_obj_key_t** obj, int* cap, int* size, ras_obj_key_t* key) {
    int idx = find_obj_key(*obj, key, *size);
    if (idx != -1) return idx;
    ras_grow_realloc_n(obj, ras_obj_key_t, cap, *size + 1);
    (*obj)[*size] =  *key;
    (*size)++; 
    return *size - 1;
}

void ras_make_obj_processed(const ras_obj_model_t* const model, ras_obj_processed_t* out) {
    *out = {};
    out->idx_count  = model->idx_count;
    int lkup_cap    = 1024;
    int lkup_size   = 0;
    const int st_count = 6; //stride count 3 coord 3 normal
    ras_obj_key_t* lkup = ras_alloc_n(ras_obj_key_t, lkup_cap);
    out->indices         = ras_alloc_n(ui32, model->idx_count);
    out->data            = ras_alloc_n(float, 6);
    out->positions       = ras_alloc_n(v4f, 1);
    out->data_count      = 0;
    ras_obj_key_t key;
    int j;
    for (int i = 0; i < model->idx_count; ++i) {
        key.position = model->data.pos[model->indices.pos[i]];
        key.coord    = model->data.coords[model->indices.coord[i]];
        key.normal   = model->data.normals[model->indices.normal[i]];
        j = find_or_insert_obj_key(&lkup, &lkup_cap, &lkup_size, &key);
        if (j >= out->data_count) {
            ras_realloc(out->data,      float, 6 * (j+1));
            ras_realloc(out->positions, v4f, j+1);
            out->data_count++;
        }
        ras_memcpy(out->positions + j,  &key.position, sizeof(key.position));
        ras_memcpy(out->data + 6*j,     &key.coord,    sizeof(float) * 3);
        ras_memcpy(out->data + 6*j + 3, &key.normal,   sizeof(float) * 3);
        out->indices[i] = j;
    }
    ras_free(lkup)
};

void ras_free_obj_processed(ras_obj_processed_t* const obj) {
   ras_free(obj->data);
   ras_free(obj->indices);
   ras_free(obj->positions);
}
