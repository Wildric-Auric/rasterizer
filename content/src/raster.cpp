#include "raster.h"
#include "ras_util.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

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
    out_framebuffer->buff   = ras_alloc_n(ui8, size.x * size.y * 4);
}

void destroy_framebuffer(ras_framebuffer_t* out_framebuffer) {
    ras_free(out_framebuffer->buff);
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
    ras_backend_resize(ras_main_framebuffer.size.x, ras_main_framebuffer.size.y);
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

void  raster_draw_prim_triangle(const ras_framebuffer_t* const framebuffer, ras_prim_triangle_t* const tri, const ras_triangle_draw_cmd_t* cmd) {
    v2i32 bx;
    v2i32 by;
    v2i32 p0 = v2i32((tri->position[0].x + 1) * framebuffer->size.x / 2.0, (tri->position[0].y + 1) * framebuffer->size.y / 2.0);
    v2i32 p1 = v2i32((tri->position[1].x + 1) * framebuffer->size.x / 2.0, (tri->position[1].y + 1) * framebuffer->size.y / 2.0);
    v2i32 p2 = v2i32((tri->position[2].x + 1) * framebuffer->size.x / 2.0, (tri->position[2].y + 1) * framebuffer->size.y / 2.0);
    v2i32 tmp = p1; 
    bx.x = ras_min3(p0.x, p1.x, p2.x);
    bx.y = ras_max3(p0.x, p1.x, p2.x);
    by.x = ras_min3(p0.y, p1.y, p2.y);
    by.y = ras_max3(p0.y, p1.y, p2.y);
    bx.x = ras_max(bx.x, 0); bx.y = ras_min(bx.y, framebuffer->size.x);
    by.x = ras_max(by.x, 0); by.y = ras_min(by.y, framebuffer->size.y);
    v2i32 v0; v2i32 v1; v2i32 v2;
    bool is_cw = ras_det2(p1 - p0, p2 - p0) < 0;
    if (is_cw) {
        p1 = p2; 
        p2 = tmp;
    }
    v0 = p1 - p0;
    v1 = p2 - p1;
    v2 = p0 - p2;
    v3f bary;
    v2i32 p;
    v3ui8 frag_col;
    int dt1; int dt2; int dt3;
    int   d;
    for (p.y = by.x; p.y < by.y; ++p.y) {
        for (p.x = bx.x; p.x < bx.y; ++p.x) {
            if ((dt1 = ras_det2(v0, p - p0)) < 0) continue;
            if ((dt2 = ras_det2(v1, p - p1)) < 0) continue;
            if ((dt3 = ras_det2(v2, p - p2)) < 0) continue;
            bary.x = (float)dt2 / (float)ras_det2(v1, p0 - p1);
            bary.y = (float)dt3 / (float)ras_det2(v2, p1 - p2);
            bary.z = 1 - bary.x - bary.y;
            switch (cmd->draw_mode) {
                case ras_triangle_draw_mode_uniform: {
                    frag_col = tri->color;
                    break;
                }
                case ras_triangle_draw_mode_bary: {
                    frag_col = v3ui8(255.0 * bary.x, 255.0 * bary.y, 255.0 * bary.z);
                    break;
                }
                case ras_triangle_draw_mode_user_func: {
                    frag_col = tri->color;
                    break;
                }
                case ras_triangle_draw_mode_wireframe: {
                    if (bary.x > cmd->wireframe_width && bary.y > cmd->wireframe_width && bary.z > cmd->wireframe_width)
                        continue;
                    frag_col = v3ui8(0,255,0);
                    break;
                }
            }
            ras_set_pixel(framebuffer, p, frag_col);
        }
    }
}

void ras_draw_triangle_list(const ras_framebuffer_t* const fmbuff, ras_triangle_list_cmd_t* cmd) {
    ras_prim_triangle_t tri;
    v4f coord_3D[3];
    float det;
    for (int i = 0; i < cmd->count; ++i) {
        tri = cmd->triangles[i]; 
        coord_3D[0] = {(float)tri.position[0].x, (float)tri.position[0].y, (float)tri.ext_3D[0].x, (float)tri.ext_3D[0].y}; 
        coord_3D[1] = {(float)tri.position[1].x, (float)tri.position[1].y, (float)tri.ext_3D[1].x,(float) tri.ext_3D[1].y}; 
        coord_3D[2] = {(float)tri.position[2].x, (float)tri.position[2].y, (float)tri.ext_3D[2].x,(float)tri.ext_3D[2].y}; 

        coord_3D[0] = cmd->transform * coord_3D[0];
        coord_3D[1] = cmd->transform * coord_3D[1];
        coord_3D[2] = cmd->transform * coord_3D[2];
        
        tri.position[0] = {(coord_3D[0].x / coord_3D[0].w), (coord_3D[0].y / coord_3D[0].w)};
        tri.position[1] = {(coord_3D[1].x / coord_3D[1].w), (coord_3D[1].y / coord_3D[1].w)};
        tri.position[2] = {(coord_3D[2].x / coord_3D[2].w), (coord_3D[2].y / coord_3D[2].w)};

        det = ras_det2(tri.position[1] - tri.position[0], tri.position[2] - tri.position[0]);
        switch (cmd->cull_mode) {
            case ras_orientation_cc: { if (det > 0) continue; break; }
            case ras_orientation_cw: { if (det < 0) continue; break; }
            default: break;
        }

        raster_draw_prim_triangle(fmbuff, &tri, &cmd->triangle_cmd);
    }
}

void ras_center_coord(const ras_framebuffer_t* fmb, float* out) {
    *out     = *out     + (fmb->size.x >> 1) ;
    *(out+1) = *(out+1) + (fmb->size.y >> 1) ;
}

template<typename scalar_g>
void ras_mat4_identity(ras_mat4<scalar_g>* m, scalar_g val) {
    m->values[0]  = val;
    m->values[1]  = 0; m->values[2] = 0; m->values[3] = 0; m->values[4] = 0;
    m->values[5]  = val;
    m->values[6]  = 0; m->values[7] = 0; m->values[8] = 0; m->values[9] = 0;
    m->values[10] = val;
    m->values[11] = 0; m->values[12] = 0; m->values[13] = 0; m->values[14] = 0;
    m->values[15] = val;
}

// ----- loop -------
#include "ras_test.h"

void raster_update() {
    //test_draw_plane();
    test_subdiv_sphere();
    //test_draw_quad();
    //test_draw_triangles();
    //test_draw_triangle();
}
