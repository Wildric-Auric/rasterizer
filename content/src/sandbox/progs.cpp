#ifndef PROGS_H
#define PROGS_H
#include "ras_math.h"
#include "ras_core.h"
void interpolate_v3_prog(const ras_frag_data_t* const frag_d, v3f* const out_color) {
    v3f x = v3f(frag_d->v_data[0][0], frag_d->v_data[0][1], frag_d->v_data[0][2]);
    v3f y = v3f(frag_d->v_data[1][0], frag_d->v_data[1][1], frag_d->v_data[1][2]);
    v3f z = v3f(frag_d->v_data[2][0], frag_d->v_data[2][1], frag_d->v_data[2][2]);
    *out_color = (x * frag_d->bary->x + y * frag_d->bary->y + z * frag_d->bary->z);
}

void texture_prog(const ras_frag_data_t* const frag_d, v3f* const out_color) {
    v2f x      = v2f(frag_d->v_data[0][0], frag_d->v_data[0][1]);
    v2f y      = v2f(frag_d->v_data[1][0], frag_d->v_data[1][1]);
    v2f z      = v2f(frag_d->v_data[2][0], frag_d->v_data[2][1]);
    v2f c      = x * frag_d->bary->x + y * frag_d->bary->y + z * frag_d->bary->z;
    float r    = c.y * 255.; 
    v2i32 n      = v2i32(c.x * 8.0, c.y * 8.0);
    if ((n.x + n.y) % 2) {
        *out_color = v3f(30,0, 20);
    }
    else {
        *out_color = v3f(255,255,255);
    }
    //*out_color = v3f(c.x*c.y*255.0, c.x*c.y*255.0, c.x*c.y*255.0);
}

#endif
