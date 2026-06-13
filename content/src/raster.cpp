#include "raster.h"
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

void raster_update() {
    static ui8 col = 0;
    ++col;
    fill_framebuffer(&ras_main_framebuffer,{255,col,0});
    ras_prim_circle_t c;
    c.radius     = 100;
    c.position.x = 0;
    c.position.y = 0;
    c.color.x = 255; c.color.y = 0; c.color.z = 0;
    c.partial = (col/255.0)*2.0*c.radius;
    raster_draw_prim_circle(&ras_main_framebuffer, &c);
}

void raster_destroy() {
    destroy_framebuffer(&ras_main_framebuffer);
}
