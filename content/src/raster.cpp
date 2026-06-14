#include "raster.h"
#include <stdlib.h>
#include <math.h>

ras_framebuffer_t ras_main_framebuffer;

void fill_framebuffer(const ras_framebuffer_t* const framebuffer, const v3ui8& color) {
    for (int i = 0; i < framebuffer->size.x * framebuffer->size.y * 4; i+=4) {
        framebuffer->buff[i+0] = color.x; 
        framebuffer->buff[i+1] = color.y; 
        framebuffer->buff[i+2] = color.z; 
        framebuffer->buff[i+3] = 255;
    }
}

void ras_set_pixel(const ras_framebuffer_t* const framebuffer, const v2i32& coord, const v3ui8& color) {
    framebuffer->buff[coord.x * 4 + 0 + coord.y * (framebuffer->size.x * 4)] = color.x;
    framebuffer->buff[coord.x * 4 + 1 + coord.y * (framebuffer->size.x * 4)] = color.y;
    framebuffer->buff[coord.x * 4 + 2 + coord.y * (framebuffer->size.x * 4)] = color.z;
    framebuffer->buff[coord.x * 4 + 3 + coord.y * (framebuffer->size.x * 4)] = 255;
}

void ras_set_pixel_safe(const ras_framebuffer_t* const framebuffer, const v2i32& coord, const v3ui8& color) {
    if (coord.x < 0 || coord.y < 0 || coord.x >= framebuffer->size.x || coord.y >= framebuffer->size.y)
        return;
    ras_set_pixel(framebuffer, coord, color);
}

void ras_get_pixel(const ras_framebuffer_t* const framebuffer, const v2i32& coord, v4ui8* const out) {
    (*out).x = *(framebuffer->buff + coord.x * 4 + 0 + coord.y * (framebuffer->size.x * 4));
    (*out).y = *(framebuffer->buff + coord.x * 4 + 1 + coord.y * (framebuffer->size.x * 4));
    (*out).z = *(framebuffer->buff + coord.x * 4 + 2 + coord.y * (framebuffer->size.x * 4));
    (*out).w = *(framebuffer->buff + coord.x * 4 + 3 + coord.y * (framebuffer->size.x * 4));
}

void init_framebuffer(ras_framebuffer_t* out_framebuffer, const v2i32& size) {
    out_framebuffer->size.x = size.x;
    out_framebuffer->size.y = size.y;
    out_framebuffer->buff   = (ui8*)malloc(size.x * size.y * 4 * sizeof(ui8));
}

void destroy_framebuffer(ras_framebuffer_t* out_framebuffer) {
    free(out_framebuffer->buff);
    out_framebuffer->buff = 0;
    out_framebuffer->size = {-1,1};
}

ras_framebuffer_t* get_main_framebuffer() {
    return &ras_main_framebuffer; 
}

void* get_raw_framebuffer() {
    return (void*)ras_main_framebuffer.buff;
}

void raster_init() {
    init_framebuffer(&ras_main_framebuffer, {512, 512});
}

void raster_destroy() {
    destroy_framebuffer(&ras_main_framebuffer);
}

void raster_draw_prim_circle(const ras_framebuffer_t* const framebuffer, const ras_prim_circle_t* const circle) {
    v2i32 tmp;
    v2i32 s  = v2i32((framebuffer->size.x>>1) + circle->position.x, (framebuffer->size.y>>1) + circle->position.y);
    int b;
    for (int y = -circle->radius; y < circle->radius && y+circle->radius < circle->partial; y++) {
        b = sqrt(pow(circle->radius, 2.0) - pow(circle->radius < 0 ? circle->radius - y : y, 2.0));
        for (int x = -b; x < b; x++) { 
            ras_set_pixel_safe(framebuffer, {x+s.x,y+s.y}, circle->color);
        }
    }
}

void  raster_draw_prim_triangle(const ras_framebuffer_t* const framebuffer, ras_prim_triangle_t* const tri) {
    v2i32 bx;
    v2i32 by;
    v2i32 tmp = tri->position[1]; 
    bx.x = ras_min3(tri->position[0].x, tri->position[1].x, tri->position[2].x);
    bx.y = ras_max3(tri->position[0].x, tri->position[1].x, tri->position[2].x);
    by.x = ras_min3(tri->position[0].y, tri->position[1].y, tri->position[2].y);
    by.y = ras_max3(tri->position[0].y, tri->position[1].y, tri->position[2].y);
    bx.x = ras_max(bx.x, 0); bx.y = ras_min(bx.y, framebuffer->size.x);
    by.x = ras_max(by.x, 0); by.y = ras_min(by.y, framebuffer->size.y);
    v2i32 v0; v2i32 v1; v2i32 v2;
    bool is_cw = ras_det2(tri->position[1] - tri->position[0], tri->position[2] - tri->position[0]) < 0;
    if (is_cw) {
        tri->position[1] = tri->position[2]; 
        tri->position[2] = tmp;
    }
    v0 = tri->position[1] - tri->position[0];
    v1 = tri->position[2] - tri->position[1];
    v2 = tri->position[0] - tri->position[2];
    v2i32 p;
    int   d;
    for (p.y = by.x; p.y < by.y; ++p.y) {
        for (p.x = bx.x; p.x < bx.y; ++p.x) {
            if (ras_det2(v0, p - tri->position[0]) < 0) continue;
            if (ras_det2(v1, p - tri->position[1]) < 0) continue;
            if (ras_det2(v2, p - tri->position[2]) < 0) continue;
            ras_set_pixel(framebuffer, p, tri->color);
        }
    }
}

void ras_draw_triangle_list(const ras_framebuffer_t* const fmbuff, ras_triangle_list_cmd_t* cmd) {
    ras_prim_triangle_t tri;
    int det;
    for (int i = 0; i < cmd->count; ++i) {
        tri = cmd->triangles[i]; 
        det = ras_det2(tri.position[1] - tri.position[0], tri.position[2] - tri.position[0]);
        switch (cmd->cull_mode) {
            case ras_orientation_cc: { if (det > 0) continue; break; }
            case ras_orientation_cw: { if (det < 0) continue; break; }
            default: break;
        }
        raster_draw_prim_triangle(fmbuff, &tri);
    }
}

void ras_center_coord(const ras_framebuffer_t* fmb, int* out) {
    *out     = *out     + (fmb->size.x >> 1) ;
    *(out+1) = *(out+1) + (fmb->size.y >> 1) ;
}
// ----- loop -------
#include "ras_test.h"

void raster_update() {
    test_draw_triangles();
    //test_draw_triangle();
}
