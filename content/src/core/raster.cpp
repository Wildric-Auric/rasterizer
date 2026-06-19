#include "raster.h"
#include "ras_util.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

ras_framebuffer_t ras_main_framebuffer;
ras_gfx_ctx_t     gfx_ctx = {{0}};

void ras_fill_framebuffer(const ras_framebuffer_t* const framebuffer, const v3ui8& color) {
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

void ras_init_framebuffer(ras_framebuffer_t* out_framebuffer, const v2i32& size) {
    out_framebuffer->size.x = size.x;
    out_framebuffer->size.y = size.y;
    out_framebuffer->buff   = ras_alloc_n(ui8, size.x * size.y * 4);
}

void ras_destroy_framebuffer(ras_framebuffer_t* out_framebuffer) {
    ras_free(out_framebuffer->buff);
    out_framebuffer->size = v2i32(-1,1);
}

ras_framebuffer_t* ras_get_main_framebuffer() {
    return &ras_main_framebuffer; 
}

void* ras_get_raw_framebuffer() {
    return (void*)ras_main_framebuffer.buff;
}

void ras_destroy() {
    ras_destroy_framebuffer(&ras_main_framebuffer);
}

void ras_draw_prim_circle(const ras_framebuffer_t* const framebuffer, const ras_prim_circle_t* const circle) {
    v2i32 tmp;
    v2i32 s  = v2i32((framebuffer->size.x>>1) + circle->position.x, (framebuffer->size.y>>1) + circle->position.y);
    int b;
    for (int y = -circle->radius; y < circle->radius && y+circle->radius < circle->partial; y++) {
        b = ras_sqrt(ras_pow(circle->radius, 2.0) - ras_pow(circle->radius < 0 ? circle->radius - y : y, 2.0));
        for (int x = -b; x < b; x++) { 
            ras_set_pixel_safe(framebuffer, v2i32(x+s.x,y+s.y), circle->color);
        }
    }
}

void  ras_draw_prim_triangle(const ras_triangle_draw_data_t* const arg) {
    v2i32 bx;
    v2i32 by;
    float* v0d = arg->tri_data.data + arg->tri_data.stride_count * 0;
    float* v1d = arg->tri_data.data + arg->tri_data.stride_count * 1;
    float* v2d = arg->tri_data.data + arg->tri_data.stride_count * 2;
    v2i32 p0 = v2i32((arg->tri->position[0].x + 1) * arg->framebuffer->size.x / 2.0, (arg->tri->position[0].y + 1) * arg->framebuffer->size.y / 2.0);
    v2i32 p1 = v2i32((arg->tri->position[1].x + 1) * arg->framebuffer->size.x / 2.0, (arg->tri->position[1].y + 1) * arg->framebuffer->size.y / 2.0);
    v2i32 p2 = v2i32((arg->tri->position[2].x + 1) * arg->framebuffer->size.x / 2.0, (arg->tri->position[2].y + 1) * arg->framebuffer->size.y / 2.0);
    v2i32 tmp    = p1; 
    float* tmp_d = v1d;
    bx.x = ras_min3(p0.x, p1.x, p2.x);
    bx.y = ras_max3(p0.x, p1.x, p2.x);
    by.x = ras_min3(p0.y, p1.y, p2.y);
    by.y = ras_max3(p0.y, p1.y, p2.y);
    bx.x = ras_max(bx.x, 0); bx.y = ras_min(bx.y, arg->framebuffer->size.x);
    by.x = ras_max(by.x, 0); by.y = ras_min(by.y, arg->framebuffer->size.y);
    v2i32 v0; v2i32 v1; v2i32 v2;
    bool is_cw = ras_det2(p1 - p0, p2 - p0) < 0;
    if (is_cw) {
        p1 = p2; 
        p2 = tmp;
        v1d = v2d;
        v2d = tmp_d;
    }
    v0 = p1 - p0;
    v1 = p2 - p1;
    v2 = p0 - p2;
    v3f bary;
    v2i32 p;
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
            switch (arg->cmd->draw_mode) {
                case ras_triangle_draw_mode_uniform: {
                    v3ui8 frag_col = v3ui8(255.0 * (arg->tri->position->z) * 0.5);
                    frag_col.y = frag_col.x;
                    frag_col.z = frag_col.x;
                    ras_set_pixel(arg->framebuffer, p, frag_col);
                    break;
                }
                case ras_triangle_draw_mode_bary: {
                    ras_set_pixel(arg->framebuffer, p, v3ui8(255.0 * bary.x, 255.0 * bary.y, 255.0 * bary.z));
                    break;
                }
                case ras_triangle_draw_mode_user_func: {
                    v3f color;
                    float* vd[3] = {v0d, v1d, v2d};
                    gfx_ctx.frag_prgs[arg->cmd->frag_prg](&bary, vd, &color);
                    ras_set_pixel(arg->framebuffer, p, v3ui8(color.x, color.y, color.z));
                    break;
                }
                case ras_triangle_draw_mode_wireframe: {
                    if (bary.x > arg->cmd->wireframe_width && bary.y > arg->cmd->wireframe_width && bary.z > arg->cmd->wireframe_width)
                        continue;
                    ras_set_pixel(arg->framebuffer, p, v3ui8(0,255,0));
                    break;
                }
            }
        }
    }
}

struct clip_data_2_t {
    ras_prim_triangle_t* tri;
    ras_prim_triangle_t* new_tri;
    float*               fp;
    int                  out_id;
    int                  id_0;
    int                  id_1;

    int                  stride_count;
    float*               tri_data; 
    float*               new_tri_data; 
    float*               tmp_stride;
};

//ras_prim_triangle_t* tri, float* fp, int out_id, int in_id
struct clip_data_1_t {
   ras_prim_triangle_t* tri;
   float*   fp;  
   int      out_id;
   int      in_id;

    int                  stride_count;
    float*               tri_data; 
    float*               tmp_stride;
};


void ras_cp_strides(float* dst, float* src, int dst_idx, int src_idx, int stride_count) {
    memcpy(dst + (stride_count * 3 * dst_idx), src + (stride_count * 3) * src_idx, stride_count * 3 * sizeof(float));
}
void ras_cp_one_stride(float* dst, float* src, int dst_idx, int src_idx, int stride_count) {
    memcpy(dst + (stride_count * dst_idx), src + stride_count * src_idx, stride_count * sizeof(float));
}

static void clip_intersect_plane_2(clip_data_2_t* arg) {
    float t0 = arg->fp[arg->out_id] / (arg->fp[arg->out_id] - arg->fp[arg->id_0]);
    float t1 = arg->fp[arg->out_id] / (arg->fp[arg->out_id] - arg->fp[arg->id_1]);
    v4f   p0 = (1.0f - t0) * arg->tri->position[arg->out_id] + t0 * arg->tri->position[arg->id_0];
    v4f   p1 = (1.0f - t1) * arg->tri->position[arg->out_id] + t1 * arg->tri->position[arg->id_1];
    //NOTE: i dont like these two allocations here can we avoid them, or at least put them outside like tmp_stride?
    float* id_0_data = ras_alloc_n(float, arg->stride_count);
    float* id_1_data = ras_alloc_n(float, arg->stride_count);
    ras_cp_one_stride(id_0_data, arg->tri_data, 0, arg->id_0,  arg->stride_count);
    ras_cp_one_stride(id_1_data, arg->tri_data, 0, arg->id_1, arg->stride_count);

    float out_d;
    float in_d;   
    int i;
    for (i = 0; i < arg->stride_count; ++i) { 
        out_d = *(arg->tri_data + arg->out_id * arg->stride_count + i);
        in_d  = *(arg->tri_data + arg->id_0   * arg->stride_count + i);
        *(arg->tmp_stride + i) = (1.0f - t0) * out_d + t0 * in_d;
    }
    for (i = 0; i < arg->stride_count; ++i) { 
        out_d = *(arg->tri_data + arg->out_id * arg->stride_count + i);
        in_d  = *(arg->tri_data + arg->id_1   * arg->stride_count + i);
        *(arg->tmp_stride + i + arg->stride_count) = (1.0f - t1) * out_d + t1 * in_d;
    }

    ras_cp_one_stride(arg->new_tri_data, id_0_data, 0, 0, arg->stride_count);
    ras_cp_one_stride(arg->new_tri_data, arg->tmp_stride, 1, 0, arg->stride_count);
    ras_cp_one_stride(arg->new_tri_data, id_1_data, 2, 0, arg->stride_count);

    ras_cp_one_stride(arg->tri_data, id_1_data, 0, 0, arg->stride_count);
    ras_cp_one_stride(arg->tri_data, arg->tmp_stride, 1, 0, arg->stride_count);
    ras_cp_one_stride(arg->tri_data, arg->tmp_stride, 2, 1, arg->stride_count);

    (arg->new_tri)->position[0] = arg->tri->position[arg->id_0];
    (arg->new_tri)->position[1] = p0;
    (arg->new_tri)->position[2] = arg->tri->position[arg->id_1];

    arg->tri->position[0] = arg->tri->position[arg->id_1];
    arg->tri->position[1] = p0;
    arg->tri->position[2] = p1;
    ras_free(id_0_data);
    ras_free(id_1_data);
}

static void clip_intersect_plane_1(clip_data_1_t* arg) {
    float t = arg->fp[arg->out_id] / (arg->fp[arg->out_id] - arg->fp[arg->in_id]);
    float ct = 1.0f - t;
    arg->tri->position[arg->out_id] = ct * arg->tri->position[arg->out_id] + t * arg->tri->position[arg->in_id];

    float out_d;
    float in_d;   
    int i;
    for (i = 0; i < arg->stride_count; ++i) { 
        out_d = *(arg->tri_data + arg->out_id * arg->stride_count + i);
        in_d  = *(arg->tri_data + arg->in_id  * arg->stride_count + i);
        *(arg->tmp_stride + i) = ct * out_d + t * in_d;
    }
    ras_cp_one_stride(arg->tri_data, arg->tmp_stride, arg->out_id, 0, arg->stride_count);
}


static void clip_full(ras_prim_triangle_t* tris, int* lkup, ras_triangle_data_t* tris_data, const ras_triangle_list_cmd_t* const cmd) {
    static const v4f clip_eqs[]     = { 
                                          v4f(1.f, 0.f, 0.f, 1.f), v4f(-1.f, 0.f, 0.f, 1.f),
                                          //v4f(0.f, 1.f, 0.f, 1.f), v4f(0.f, -1.f, 0.f, 1.f),
                                          v4f(0.f, 0.f, 1.f, 1.f), v4f(0.f, 0.f, -1.f, 1.f),
                                      };
    static const int clip_eqs_count = sizeof(clip_eqs) /  sizeof(clip_eqs[0]);

    clip_data_2_t        clip_d2{};
    clip_data_1_t        clip_d1{};
    clip_d2.stride_count = tris_data->stride_count;
    clip_d1.stride_count = tris_data->stride_count;
    if (tris_data->stride_count) {
        clip_d1.tmp_stride = ras_alloc_n(float, tris_data->stride_count * 3);
        clip_d2.tmp_stride = clip_d1.tmp_stride;
    }
    ras_prim_triangle_t* tri;
    float ev[3];
    ui8   mask;
    int x   = 0;
    int inc = 1;
    for (int i = 0; i < clip_eqs_count; ++i) {
        while(1) {
            x = -1;
            for (int r = 0; r < 24; ++r) { if (lkup[r] == i+1) {x = r; break;} }
            if (x == -1) {
                for (int r = 0; r < 24; ++r) { if (lkup[r] == i+1) {lkup[r] = i+2;}}
                break;
            }
            tri = tris + x; 

            clip_d1.tri = tri; 
            clip_d1.fp = ev; 
            clip_d1.tri_data = tris_data->data + 3 * tris_data->stride_count * x;

            clip_d2.tri_data = tris_data->data + 3 * tris_data->stride_count * x;
            clip_d2.new_tri_data = tris_data->data + 3 * inc * tris_data->stride_count;
            clip_d2.tri = tri; 
            clip_d2.new_tri = tris+inc; clip_d2.fp = ev; 
            clip_d2.fp  = ev;

            ev[0] = ras_v4f_dot(clip_eqs[i], tri->position[0]);
            ev[1] = ras_v4f_dot(clip_eqs[i], tri->position[1]);
            ev[2] = ras_v4f_dot(clip_eqs[i], tri->position[2]);
            mask  = (ev[0] < 0.f ? 1 : 0) | (ev[1] < 0.f ? 1<<1 : 0) | (ev[2] < 0.f ? 1 << 2 : 0);
            switch (mask) {
                case 0b110: {
                    //ras_prim_triangle_t* tri, float* fp, int out_id, int in_id
                    clip_d1.out_id = 1; clip_d1.in_id  = 2;
                    clip_intersect_plane_1(&clip_d1);
                    clip_d1.out_id = 2; clip_d1.in_id = 0;
                    clip_intersect_plane_1(&clip_d1);
                    lkup[x] = i+2;
                    break;
                }
                case 0b101: {
                    clip_d1.tri = tri; clip_d1.fp = ev; clip_d1.out_id = 0; clip_d1.in_id = 1;
                    clip_intersect_plane_1(&clip_d1);
                    clip_d1.out_id = 2; clip_d1.in_id = 1;
                    clip_intersect_plane_1(&clip_d1);
                    lkup[x] = i+2;
                    break;
                }
                case 0b011: {
                    clip_d1.tri = tri; clip_d1.fp = ev; clip_d1.out_id = 0; clip_d1.in_id = 2;
                    clip_intersect_plane_1(&clip_d1);
                    clip_d1.out_id = 1; clip_d1.in_id = 2;
                    clip_intersect_plane_1(&clip_d1);
                    lkup[x] = i+2;
                    break;
                }
                case 0b001: {
                    clip_d2.out_id = 0; clip_d2.id_0 = 1; clip_d2.id_1 = 2;
                    clip_intersect_plane_2(&clip_d2);
                    lkup[x]   = i+2;
                    lkup[inc] = i+2;
                    inc++;
                    break;
                }
                case 0b010: {
                    clip_d2.tri = tri; clip_d2.new_tri = tris+inc; clip_d2.fp = ev; 
                    clip_d2.out_id = 1; clip_d2.id_0 = 0; clip_d2.id_1 = 2;
                    clip_intersect_plane_2(&clip_d2);
                    lkup[x]   = i+2;
                    lkup[inc] = i+2;
                    inc++;
                    break;
                }
                case 0b100: {
                    clip_d2.tri = tri; clip_d2.new_tri = tris+inc; clip_d2.fp = ev; 
                    clip_d2.out_id = 2; clip_d2.id_0 = 0; clip_d2.id_1 = 1;
                    clip_intersect_plane_2(&clip_d2);
                    lkup[x]   = i+2;
                    lkup[inc] = i+2;
                    inc++;
                    break;
                }
                case 0: {
                    lkup[x] = i+2;
                    break;
                }
                case 0b111: {
                    lkup[x] = 0; 
                    break;
                }
            }
        }
    }
    if (clip_d1.tmp_stride)
        ras_free(clip_d1.tmp_stride);
}

void ras_draw_triangle_list(const ras_framebuffer_t* const fmbuff, const ras_triangle_list_cmd_t* const cmd) {
    ras_prim_triangle_t         tris    [24];
    ras_triangle_data_t         tris_data{};
    int                         tri_lkup[24] = {0};
    ras_prim_triangle_t*        tri = tris;
    ras_triangle_draw_data_t    draw_data{0};

    tris_data.stride_count   = cmd->tris_data.stride_count;
    if (tris_data.stride_count)
        tris_data.data       = ras_alloc_n(float, 3 * tris_data.stride_count * 24);
    
    float det;
    for (int i = 0; i < cmd->count; ++i) {
        *tri = cmd->triangles[i]; 
        tri->position[0] = cmd->transform * tri->position[0];
        tri->position[1] = cmd->transform * tri->position[1];
        tri->position[2] = cmd->transform * tri->position[2];
        
        //----- backsface culling ------ 
        det = ras_det2(ras_to_v2f((tri->position[1]/tri->position[1].w - tri->position[0]/tri->position[0].w)), 
                       ras_to_v2f(tri->position[2]/tri->position[2].w - tri->position[0]/tri->position[0].w)); //todo:: avoid doing this twice in perspective div below 
        switch (cmd->cull_mode) {
            case ras_orientation_cc: { if (det > 0) continue; break; }
            case ras_orientation_cw: { if (det < 0) continue; break; }
            default: break;
        }
        //----------clipping-------------------------
        for (int i = 0; i < 24; ++i) tri_lkup[i] = 0;
        tris[0]     = *tri;
        tri_lkup[0] = 1;
        ras_cp_strides(tris_data.data, cmd->tris_data.data, 0, i, cmd->tris_data.stride_count);
        clip_full(tris, tri_lkup, &tris_data, cmd);
        //----------perspective division-------------
        tri = tris;
        for (int t = 0; t < 24; ++t) {
            if (tri_lkup[t] == 0) {tri++; continue;}    
            tri->position[0].x    = tri->position[0].x / tri->position[0].w;
            tri->position[0].y    = tri->position[0].y / tri->position[0].w;
            tri->position[1].x    = tri->position[1].x / tri->position[1].w;
            tri->position[1].y    = tri->position[1].y / tri->position[1].w;
            tri->position[2].x    = tri->position[2].x / tri->position[2].w;
            tri->position[2].y    = tri->position[2].y / tri->position[2].w;
        //----- draw triangle -----
            draw_data.framebuffer           = fmbuff; 
            draw_data.tri_data.data         = tris_data.data + 3 * tris_data.stride_count * t;
            draw_data.tri_data.stride_count = tris_data.stride_count;
            draw_data.cmd                   = &cmd->triangle_cmd;
            draw_data.tri                   = tri;
            ras_draw_prim_triangle(&draw_data);
            tri++;
        }
        tri = tris;
    }
    if (tris_data.stride_count)
        ras_free(tris_data.data);
}

void ras_register_frag_program(frag_prg_proc proc,const int idx) {
    gfx_ctx.frag_prgs[idx] = proc;
}

