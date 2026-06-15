#include "raster.h"
#include "ras_util.h"
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
    v2f tr        = {0   , 0};
    tri.position[0] = {-1.f,-1.f};
    tri.position[1] = {1.f ,-1.f};
    tri.position[2] = {0   , 1.f};

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
        tri->position[0] = v2f(i*0.05,0) + v2f(0,0);
        tri->position[1] = v2f(i*0.05,0) + v2f(0.05,0);
        tri->position[2] = v2f(i*0.05,0) + v2f(0,0.05);
        tri->ext_3D  [0] = {0,1};
        tri->ext_3D  [1] = {0,1};
        tri->ext_3D  [2] = {0,1};
        tri->color       = {255,0,0};
    }
    v2i32 mouse;
    m4f   rot_m; 
    m4i32   scale_m;
    m4i32 tr_m;
    ras_backend_get_mouse(&mouse.x, &mouse.y);
    ras_set_m4_diag(&cmd.transform, 1.0f);
    ras_set_m4_diag(&rot_m, 1.0f);
    ras_set_m4_diag(&scale_m, 1);
    ras_set_m4_diag(&tr_m, 1);

    ras_m4_set_rot_z(&rot_m, 3.14 / 4);
    ras_m4_scale(&scale_m, v3i32(2,2,1));
    ras_m4_translate(&tr_m, v3i32(mouse.x, mouse.y, 0));

    cmd.transform = tr_m * scale_m * rot_m ;

    cmd.triangle_cmd.draw_mode = ras_triangle_draw_mode_bary;
    //---clockwise test---
    tri = cmd.triangles + 4;
    tri->position[0] = v2f(0,0.05);
    tri->position[2] = v2f(0.05,0.05);
    tri->position[1] = v2f(0,0.1);
    tri->ext_3D  [0] = {0,1};
    tri->ext_3D  [1] = {0,1};
    tri->ext_3D  [2] = {0,1};
    tri->color       = {0,0,255};

    fill_framebuffer(fmbuff, {255,255,0});
    ras_draw_triangle_list(fmbuff, &cmd); 
    ras_free(cmd.triangles);
}

void test_draw_quad() {
    ras_framebuffer_t* fmbuff = get_main_framebuffer();
    ras_triangle_list_cmd_t cmd;
    cmd.cull_mode = ras_orientation_none;
    cmd.count     = 12 ;
    cmd.triangles = ras_alloc_n(ras_prim_triangle_t, cmd.count);

#define def_tri(i, j, x, y, z) \
    cmd.triangles[i].position[j]=v2f(x,y); cmd.triangles[i].ext_3D[j]=v2f(z,1);

    def_tri(0,0,-1,-1,-1);
    def_tri(0,1, 1,-1,-1);
    def_tri(0,2,-1, 1,-1);
    def_tri(1,0, 1,-1,-1);
    def_tri(1,1, 1, 1,-1);
    def_tri(1,2,-1, 1,-1);

    def_tri(2,0, 1,-1,1);
    def_tri(2,1,-1,-1,1);
    def_tri(2,2,-1, 1,1);
    def_tri(3,0, 1, 1,1);
    def_tri(3,1, 1,-1,1);
    def_tri(3,2,-1, 1,1);

    def_tri(4,0,1,-1,-1);
    def_tri(4,1,1,-1, 1);
    def_tri(4,2,1, 1,-1);
    def_tri(5,0,1,-1, 1);
    def_tri(5,1,1, 1, 1);
    def_tri(5,2,1, 1,-1);

    def_tri(6,0,-1,-1, 1);
    def_tri(6,1,-1,-1,-1);
    def_tri(6,2,-1, 1,-1);
    def_tri(7,0,-1, 1, 1);
    def_tri(7,1,-1,-1, 1);
    def_tri(7,2,-1, 1,-1);

    def_tri(8,0,-1,1,-1);
    def_tri(8,1, 1,1,-1);
    def_tri(8,2,-1,1, 1);
    def_tri(9,0, 1,1,-1);
    def_tri(9,1, 1,1, 1);
    def_tri(9,2,-1,1, 1);

    def_tri(10,0,-1, -1,1);
    def_tri(10,1,1,  -1,1);
    def_tri(10,2,-1, -1,-1);
    def_tri(11,0,-1, -1,-1);
    def_tri(11,1, 1, -1,1);
    def_tri(11,2, 1, -1,-1);
#undef def_tri

    v2i32 mouse;

    m4f   proj_m; 
    m4f   model_m;
    m4f   view_m;
    
    m4f   rot_m; 
    m4f   rot_m2; 
    m4f scale_m;
    m4f tr_m;
    ras_set_m4_diag(&cmd.transform, 1.0f);
    ras_set_m4_diag(&rot_m,   1.0f);
    ras_set_m4_diag(&rot_m2,   1.0f);
    ras_set_m4_diag(&scale_m, 1.0f);
    ras_set_m4_diag(&tr_m,    1.0f);
    ras_set_m4_diag(&proj_m, 1.0f);
    ras_set_m4_diag(&view_m, 1.0f);
    ras_set_m4_diag(&model_m, 1.0f);

    static float x = 0;
    x += 0.001;

    ras_m4_set_rot_x(&rot_m,  -x * 3.14);
    ras_m4_set_rot_y(&rot_m2, -x * 3.14);
    ras_m4_scale(&scale_m, v3i32(2,2,2));
    ras_m4_translate(&tr_m, v3i32(0, 0, 0));
    ras_m4_perspective(&proj_m, 70.0f * 3.1415f / 180.0f, (float)fmbuff->size.y / fmbuff->size.x, 1.0f, 124.0f);
    
    ras_m4_translate(&tr_m, v3f(0.0));

    ras_m4_translate(&view_m, v3f(0, 0, 10));

    rot_m   = rot_m2 * rot_m;
    model_m = tr_m * scale_m * rot_m;
    cmd.transform = proj_m * view_m * model_m;
    cmd.triangle_cmd.draw_mode = ras_triangle_draw_mode_wireframe;
    fill_framebuffer(fmbuff, {25,25,25});
    ras_draw_triangle_list(fmbuff, &cmd); 
    ras_free(cmd.triangles);
}

void test_draw_plane() {
    ras_framebuffer_t* fmbuff = get_main_framebuffer();
    ras_triangle_list_cmd_t cmd;
    cmd.cull_mode = ras_orientation_cw;
    cmd.count     = 2;
    cmd.triangles = ras_alloc_n(ras_prim_triangle_t, cmd.count);
    cmd.count     = 2;

    cmd.triangles[0].position[0] = v2f(-1,-1);
    cmd.triangles[0].position[1] = v2f(1, -1);
    cmd.triangles[0].position[2] = v2f(-1,  1);

    cmd.triangles[1].position[0] = v2f(1, 1);
    cmd.triangles[1].position[1] = v2f(-1, 1);
    cmd.triangles[1].position[2] = v2f(1, -1);

    cmd.triangles[0].ext_3D[0] = v2f(0,1);
    cmd.triangles[0].ext_3D[1] = v2f(0,1);
    cmd.triangles[0].ext_3D[2] = v2f(0,1);
    cmd.triangles[1].ext_3D[0] = v2f(0,1);
    cmd.triangles[1].ext_3D[1] = v2f(0,1);
    cmd.triangles[1].ext_3D[2] = v2f(0,1);

    v2i32 mouse;

    m4f   proj_m; 
    m4f   model_m;
    m4f   view_m;
    
    m4f   rot_m; 
    m4f   scale_m;
    m4f   tr_m;
    ras_set_m4_diag(&cmd.transform, 1.0f);
    ras_set_m4_diag(&rot_m, 1.0f);
    ras_set_m4_diag(&scale_m, 1.0f);
    ras_set_m4_diag(&tr_m, 1.0f);
    ras_set_m4_diag(&proj_m, 1.0f);
    ras_set_m4_diag(&view_m, 1.0f);
    ras_set_m4_diag(&model_m, 1.0f);

    static float x = 0;
    x += 0.01;

    ras_m4_set_rot_x(&rot_m, 0.0);
    ras_m4_scale(&scale_m, v3i32(1,1,1));
    ras_m4_translate(&tr_m, v3i32(0, 0, 0));
    ras_m4_perspective(&proj_m, 70.0f * 3.1415f / 180.0f, (float)fmbuff->size.y / fmbuff->size.x, 1.0f, 124.0f);
    
    ras_m4_translate(&tr_m, v3f(0.0));

    ras_m4_translate(&view_m, v3f(0, 0, -10));

    model_m = tr_m * scale_m * rot_m;
    cmd.transform = proj_m * view_m * model_m;
    cmd.triangle_cmd.draw_mode = ras_triangle_draw_mode_bary;

    fill_framebuffer(fmbuff, {255,255,0});
    ras_draw_triangle_list(fmbuff, &cmd); 
    ras_free(cmd.triangles);
}
