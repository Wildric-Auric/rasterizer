#ifndef RASTER_H
#define RASTER_H

#include "ras_math.h"

struct ras_framebuffer_t {
   ui8*   buff;
   v2i32  size;
};

struct ras_prim_circle_t {
    float radius;
    int   partial;
    v2i32 position;
    v3ui8 color;
};

enum ras_triangle_draw_mode_ : ui8 {
    ras_triangle_draw_mode_uniform       = 0,
    ras_triangle_draw_mode_wireframe     = 1,
    ras_triangle_draw_mode_bary          = 2,
    ras_triangle_draw_mode_user_func     = 3,
};

struct ras_prim_triangle_t {
    v4f   position[3];
};

enum ras_orientation_ {
    ras_orientation_none,
    ras_orientation_cw,
    ras_orientation_cc
};

struct ras_triangle_draw_cmd_t {
    int frag_prg          = 0;
    ras_triangle_draw_mode_ draw_mode;
    float wireframe_width = 0.009;
};

struct ras_triangle_data_t {
    float*  data; 
    int     stride_count;
};

struct ras_triangle_list_cmd_t {
    ras_orientation_        cull_mode; 
    ras_prim_triangle_t*    triangles;
    ras_triangle_data_t     tris_data;
    int                     count;
    m4f                     transform;
    ras_triangle_draw_cmd_t triangle_cmd;
};

struct ras_triangle_draw_data_t {
    const ras_framebuffer_t*       framebuffer; 
    const ras_prim_triangle_t*     tri;
    const ras_triangle_draw_cmd_t* cmd;
    ras_triangle_data_t            tri_data;
};

struct ras_frag_data_t {
    const v3f*   bary;
    const v4f*   v_pos[3];
    const float* v_data[3];
};

typedef void (*frag_prg_proc)(const ras_frag_data_t* const data, v3f* const out_color);

struct ras_gfx_ctx_t {
    frag_prg_proc frag_prgs[256];
};


// -------- Ctx methods --------
ras_framebuffer_t* ras_get_main_framebuffer();
void               ras_init();
void               ras_update();
void               ras_destroy(); 
void               ras_backend_get_mouse(int*, int*);
void               ras_backend_resize(int, int);
// -------- Frame buffer methods --------
void               ras_fill_framebuffer(const ras_framebuffer_t* const, const v3ui8&);
void               ras_init_framebuffer(ras_framebuffer_t* out_framebuffer, const v2i32& size);
void               ras_destroy_framebuffer(ras_framebuffer_t* out_framebuffer);
void               ras_set_pixel(const ras_framebuffer_t* const framebuffer, const v2i32& coord, const v3ui8& color);
void               ras_set_pixel_safe(const ras_framebuffer_t* const framebuffer, const v2i32& coord, const v3ui8& color);
void               ras_get_pixel(const ras_framebuffer_t* const framebuffer, const v2i32& coord, v4ui8* out);
void*              ras_get_raw_framebuffer();
void               ras_register_frag_program(frag_prg_proc,const int);
//  ------- Raster methods ----------
void               ras_draw_prim_circle(const ras_framebuffer_t* const, const ras_prim_circle_t* const);
void               ras_draw_prim_triangle(const ras_triangle_draw_data_t* const);
void               ras_draw_triangle_list(const ras_framebuffer_t* const, const ras_triangle_list_cmd_t* const);

#endif
