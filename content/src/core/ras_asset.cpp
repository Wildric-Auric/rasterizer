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
    int vert_count = 0;
    int face_count = 0;
    int vert_cap = 1024;
    int face_cap = 1024; 
    model_parse_state_ state = model_parse_state_general;
    v3f vertex;
    v3f*   verts = ras_alloc_n(v3f,      vert_cap);
    v3i32* faces = ras_alloc_n(v3i32,    face_cap);
    v3i32  tri;
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
                    vert_count++;
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
                ras_grow_realloc_n(faces, v3i32, face_cap, face_count+1); 
                faces[face_count] = tri;
                tri.y = tri.z;
                face_count++;
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
                ras_grow_realloc_n(faces, v3i32, face_cap, face_count+1);
                faces[face_count] = tri;
                tri.y = tri.z;
                face_count++;
                if (endl)
                    state = model_parse_state_general;
                break;
            }
            case model_parse_state_vertex: {
                const int cur_idx = vert_count-1;
                parse_word(f, wrd, &endl, &r);
                vertex.x = atof(wrd);
                parse_word(f, wrd, &endl, &r);
                vertex.y = atof(wrd);
                parse_word(f, wrd, &endl, &r);
                vertex.z = atof(wrd); 
                ras_grow_realloc_n(verts, v3f, vert_cap, vert_count);
                verts[cur_idx] = vertex;
                state = model_parse_state_general;
                break;
            }
            default: break;
        }
    }
    v3f* tris = ras_alloc_n(v3f, face_count * 3);
    for (int i = 0; i < face_count; i += 1) {
        tris[i*3 + 0] = verts[faces[i].x];
        tris[i*3 + 1] = verts[faces[i].z];
        tris[i*3 + 2] = verts[faces[i].y];
    }
    ras_free(verts);
    ras_free(faces);
    out_data->triangles = (float*)tris;
    out_data->count     = face_count;
    return 0;
}

void ras_free_obj_model(void** data) {
    if (!*data) return;
    ras_free(*data);
}
