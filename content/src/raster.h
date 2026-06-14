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
    v2i32 position[3];
    v2i32 ext_3D[3];
    v3ui8 color;
};

enum ras_orientation_ {
    ras_orientation_none,
    ras_orientation_cw,
    ras_orientation_cc
};

struct ras_triangle_draw_cmd_t {
    ras_triangle_draw_mode_ draw_mode;
};

struct ras_triangle_list_cmd_t {
    ras_orientation_        cull_mode; 
    ras_prim_triangle_t*    triangles;
    int                     count;
    m4i32                   transform;
    ras_triangle_draw_cmd_t triangle_cmd;
};


// -------- Ctx methods --------
ras_framebuffer_t* get_main_framebuffer();
void               raster_init();
void               raster_update();
void               raster_destroy(); 
void               ras_backend_get_mouse(int*, int*);
void               ras_backend_resize(int, int);
// -------- Frame buffer methods --------
void               fill_framebuffer(const ras_framebuffer_t* const, const v3ui8&);
void               init_framebuffer(ras_framebuffer_t* out_framebuffer, const v2i32& size);
void               destroy_framebuffer(ras_framebuffer_t* out_framebuffer);
void               ras_set_pixel(const ras_framebuffer_t* const framebuffer, const v2i32& coord, const v3ui8& color);
void               ras_set_pixel_safe(const ras_framebuffer_t* const framebuffer, const v2i32& coord, const v3ui8& color);
void               ras_get_pixel(const ras_framebuffer_t* const framebuffer, const v2i32& coord, v4ui8* out);
void*              get_raw_framebuffer();
//  ------- Raster methods ----------
void               ras_center_coord(const ras_framebuffer_t*, int* out);
void               raster_draw_prim_circle(const ras_framebuffer_t* const, const ras_prim_circle_t* const);
void               raster_draw_prim_triangle(const ras_framebuffer_t* const, ras_prim_triangle_t* const, const ras_triangle_draw_cmd_t* const);
void               ras_draw_triangle_list(const ras_framebuffer_t* const, ras_triangle_list_cmd_t*);

#endif
