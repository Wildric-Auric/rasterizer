#include "raster.h"
#include <stdlib.h>

void test_draw_circle() {
    static ui8 col = 0;
    ++col;
    fill_framebuffer(get_main_framebuffer(),{255,col,0});
    ras_prim_circle_t c;
    c.radius     = 100;
    c.position.x = 0;
    c.position.y = 0;
    c.color.x = 255; c.color.y = 0; c.color.z = 0;
    c.partial = (col/255.0)*2.0*c.radius;
    raster_draw_prim_circle(get_main_framebuffer(), &c);
}

void test_draw_triangle() {
    ras_framebuffer_t* fmbuff = get_main_framebuffer();
    ras_prim_triangle_t tri;
    v2i32 tr        = {0   , 0};
    tri.position[0] = {-100,-100};
    tri.position[1] = {100 ,-100};
    tri.position[2] = {0   , 100};

    tri.position[0] += tr;
    tri.position[1] += tr;
    tri.position[2] += tr;

    ras_center_coord(fmbuff, &tri.position[0].x);
    ras_center_coord(fmbuff, &tri.position[1].x);
    ras_center_coord(fmbuff, &tri.position[2].x);
    tri.color       = {0,0,255};
    fill_framebuffer(fmbuff, {255,255,255});
    raster_draw_prim_triangle(fmbuff, &tri);
}

void test_draw_triangles() {
    ras_framebuffer_t* fmbuff = get_main_framebuffer();
    ras_triangle_list_cmd_t cmd;
    cmd.cull_mode = ras_orientation_cw;
    cmd.count     = 5;
    cmd.triangles = (ras_prim_triangle_t*)malloc(cmd.count * sizeof(ras_prim_triangle_t));
    ras_prim_triangle_t* tri;
    for (int i = 0; i < 4; ++i) {
        tri = cmd.triangles + i;
        tri->position[0] = v2i32(i*32,0) + v2i32(0,0);
        tri->position[1] = v2i32(i*32,0) + v2i32(32,0);
        tri->position[2] = v2i32(i*32,0) + v2i32(0,32);
        tri->color.x     = 255;
        ras_center_coord(fmbuff, &tri->position[0].x);
        ras_center_coord(fmbuff, &tri->position[1].x);
        ras_center_coord(fmbuff, &tri->position[2].x);
    }
    //---clockwise test---
    tri = cmd.triangles + 4;
    tri->position[0] = v2i32(0,32);
    tri->position[2] = v2i32(32,32);
    tri->position[1] = v2i32(0,64);
    tri->color.z     = 255;

    ras_center_coord(fmbuff, &tri->position[0].x);
    ras_center_coord(fmbuff, &tri->position[1].x);
    ras_center_coord(fmbuff, &tri->position[2].x);
    ras_draw_triangle_list(fmbuff, &cmd); 
    free(cmd.triangles);
}
