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

ras_framebuffer_t* get_main_framebuffer();
void               raster_init();
void               raster_update();
void               raster_destroy(); 
void               fill_framebuffer(const ras_framebuffer_t* const, const v3ui8&);
void               init_framebuffer(ras_framebuffer_t* out_framebuffer, const v2i32& size);
void               destroy_framebuffer(ras_framebuffer_t* out_framebuffer);
void               ras_set_pixel(const ras_framebuffer_t* const framebuffer, const v2i32& coord, const v3ui8& color);
void               ras_set_pixel_safe(const ras_framebuffer_t* const framebuffer, const v2i32& coord, const v3ui8& color);
void               ras_get_pixel(const ras_framebuffer_t* const framebuffer, const v2i32& coord, v4ui8* out);
void*              get_raw_framebuffer();
void               raster_draw_prim_circle(const ras_framebuffer_t* const, const ras_prim_circle_t* const);

#endif
