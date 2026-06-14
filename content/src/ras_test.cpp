#include "raster.h"
#include "ras_util.h"
#include <stdlib.h>
#include <stdio.h>

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
    ras_triangle_draw_cmd_t cmd;
    cmd.draw_mode = ras_triangle_draw_mode_uniform;
    raster_draw_prim_triangle(fmbuff, &tri, &cmd);
}

void test_draw_triangles() {
    ras_framebuffer_t* fmbuff = get_main_framebuffer();
    ras_triangle_list_cmd_t cmd;
    cmd.cull_mode = ras_orientation_none;
    cmd.count     = 5;
    cmd.triangles = ras_alloc_n(ras_prim_triangle_t, cmd.count);
    ras_prim_triangle_t* tri;
    for (int i = 0; i < 4; ++i) {
        tri = cmd.triangles + i;
        tri->position[0] = v2i32(i*32,0) + v2i32(0,0);
        tri->position[1] = v2i32(i*32,0) + v2i32(32,0);
        tri->position[2] = v2i32(i*32,0) + v2i32(0,32);
        tri->ext_3D  [0] = {0,1};
        tri->ext_3D  [1] = {0,1};
        tri->ext_3D  [2] = {0,1};
        tri->color       = {255,0,0};
    }
    v2i32 mouse;
    ras_backend_get_mouse(&mouse.x, &mouse.y);
    ras_set_m4_diag(&cmd.transform, 1);
    ras_m4_scale(&cmd.transform, v3i32(2,2,1));
    ras_m4_translate(&cmd.transform, v3i32(mouse.x, mouse.y, 0));
    cmd.triangle_cmd.draw_mode = ras_triangle_draw_mode_bary;
    //---clockwise test---
    tri = cmd.triangles + 4;
    tri->position[0] = v2i32(0,32);
    tri->position[2] = v2i32(32,32);
    tri->position[1] = v2i32(0,64);
    tri->ext_3D  [0] = {0,1};
    tri->ext_3D  [1] = {0,1};
    tri->ext_3D  [2] = {0,1};
    tri->color       = {0,0,255};

    fill_framebuffer(fmbuff, {255,255,0});
    ras_draw_triangle_list(fmbuff, &cmd); 
    ras_free(cmd.triangles);
}
