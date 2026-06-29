#ifndef PROGS_H
#define PROGS_H
#include "ras_math.h"
#include "ras_core.h"
#include <stdio.h>
void interpolate_v3_prog(const ras_frag_data_t* const frag_d, ras_frag_out_data_t* out_d) {
    v3f x = v3f(frag_d->v_data[0][0], frag_d->v_data[0][1], frag_d->v_data[0][2]);
    v3f y = v3f(frag_d->v_data[1][0], frag_d->v_data[1][1], frag_d->v_data[1][2]);
    v3f z = v3f(frag_d->v_data[2][0], frag_d->v_data[2][1], frag_d->v_data[2][2]);
    out_d->color = (x * frag_d->bary->x + y * frag_d->bary->y + z * frag_d->bary->z);
}

void texture_prog(const ras_frag_data_t* const frag_d, ras_frag_out_data_t* out_d) {
    v2f x      = v2f(frag_d->v_data[0][0], frag_d->v_data[0][1]);
  
    v2f y      = v2f(frag_d->v_data[1][0], frag_d->v_data[1][1]);
    v2f z      = v2f(frag_d->v_data[2][0], frag_d->v_data[2][1]);
    v2f c      = x * frag_d->bary->x + y * frag_d->bary->y + z * frag_d->bary->z;
    float r    = c.y * 255.; 
    v3ui8 smp;
    v2i32 n      = v2i32(c.x * 8.0, c.y * 8.0);
    if ((n.x + n.y) % 2) {
        out_d->color = v3f(30,0, 20);
    }
    else {
        out_d->color = v3f(255,255,255);
    }
    //out_d->color = v3f(c.x*c.y*255.0, c.x*c.y*255.0, c.x*c.y*255.0);
}

void texture_test_prog(const ras_frag_data_t* const frag_d, ras_frag_out_data_t* out_d) {
    v2f x      = v2f(frag_d->v_data[0][0], frag_d->v_data[0][1]);
    v2f y      = v2f(frag_d->v_data[1][0], frag_d->v_data[1][1]);
    v2f z      = v2f(frag_d->v_data[2][0], frag_d->v_data[2][1]);
    v2f c      = x * frag_d->bary->x + y * frag_d->bary->y + z * frag_d->bary->z;
    float r    = c.y * 255.; 
    v3ui8 smp;
    ras_sample_texture(1, c, &smp);
    out_d->color = v3f(smp.x, smp.y, smp.z);
//    c = c * 255.0f;
//    out_d->color.x = c.x;
//    out_d->color.y = c.y;
//    out_d->color.z = 255;
}

void texture_coord_prog(const ras_frag_data_t* const frag_d, ras_frag_out_data_t* out_d) {
    v2f x      = v2f(frag_d->v_data[0][0], frag_d->v_data[0][1]);
    v2f y      = v2f(frag_d->v_data[1][0], frag_d->v_data[1][1]);
    v2f z      = v2f(frag_d->v_data[2][0], frag_d->v_data[2][1]);
    v2f c      = x * frag_d->bary->x + y * frag_d->bary->y + z * frag_d->bary->z;
    v3ui8 col;
    static int a = 0;
    if (!a) {
        printf("%f %f %f\n", x.x, y.x, z.x);
        a = 1;
    }
    ras_sample_texture(1, c, &col);
    out_d->color = v3f(col.x, col.y, col.z);
//    c = c * 255.0f;
//    out_d->color.x = c.x;
//    out_d->color.y = c.y;
//    out_d->color.z = 255;
}

#endif
