#ifndef PROGS_H
#define PROGS_H
#include "ras_math.h"
void interpolate_v3_prog(const v3f* const bary, float* const v_data[3], v3f* const out_color) {
    v3f x = v3f(v_data[0][0], v_data[0][1], v_data[0][2]);
    v3f y = v3f(v_data[1][0], v_data[1][1], v_data[1][2]);
    v3f z = v3f(v_data[2][0], v_data[2][1], v_data[2][2]);
    *out_color = x * bary->x + y * bary->y + z * bary->z;
}

void texture_prog(const v3f* const bary, float* const v_data[3], v3f* const out_color) {
    v2f x      = v2f(v_data[0][0], v_data[0][1]);
    v2f y      = v2f(v_data[1][0], v_data[1][1]);
    v2f z      = v2f(v_data[2][0], v_data[2][1]);
    v2f c      = x * bary->x + y * bary->y + z * bary->z;
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
