#include "ras_test.h"
#include "ras_core.h"
#include "ras_util.h"
#include <stdlib.h>

void test_draw_circle() {
    static ui8 col = 0;
    ++col;
    ras_fill_framebuffer(ras_get_main_framebuffer(), v3ui8(255,col,0));
    ras_prim_circle_t c;
    c.radius     = 100;
    c.position.x = 0;
    c.position.y = 0;
    c.color.x = 255; c.color.y = 0; c.color.z = 0;
    c.partial = (col/255.0)*2.0*c.radius;
    ras_draw_prim_circle(ras_get_main_framebuffer(), &c);
}

void test_draw_triangle() {
    ras_framebuffer_t* fmbuff = ras_get_main_framebuffer();
    ras_prim_triangle_t tri;
    v4f tr(0,0,0,0);
    tri.position[0] = v4f(-1.f,-1.f);
    tri.position[1] = v4f(1.f ,-1.f);
    tri.position[2] = v4f(0   , 1.f);

    tri.position[0] += tr;
    tri.position[1] += tr;
    tri.position[2] += tr;

    //tri.color       = v3ui8(0,0,255);
    ras_fill_framebuffer(fmbuff, v3ui8(255,255,255));
    ras_triangle_draw_cmd_t cmd{};
    ras_triangle_draw_data_t d;
    cmd.draw_mode = ras_triangle_draw_mode_uniform;
    d.tri         = &tri;
    d.framebuffer = fmbuff;
    d.cmd           = &cmd;
    d.tri_data.data = 0;
    d.tri_data.stride_count = 0;
    ras_draw_prim_triangle(&d);
}

void test_draw_triangles() {
    ras_framebuffer_t* fmbuff = ras_get_main_framebuffer();
    ras_triangle_list_cmd_t cmd{};
    cmd.cull_mode = ras_orientation_none;
    cmd.count     = 5;
    cmd.triangles = ras_alloc_n(ras_prim_triangle_t, cmd.count);
    ras_prim_triangle_t* tri;
    for (int i = 0; i < 4; ++i) {
        tri = cmd.triangles + i;
        tri->position[0] = v4f(i,0,0,1) + v4f(0,0);
        tri->position[1] = v4f(i,0,0,1) + v4f(1.0,0,0);
        tri->position[2] = v4f(i,0,0,1) + v4f(0,1.0);
        //tri->color       = v3ui8(255,0,0);
    }
    v2i32 mouse;
    m4f   rot_m(1.0f); 
    m4f   scale_m(1.0f);
    m4f tr_m(1.0f);
    ras_backend_get_mouse(&mouse.x, &mouse.y);
    ras_set_m4_diag(&cmd.transform, 1.0f);

    ras_m4_set_rot_z(&rot_m, 3.14 / 4);
    ras_m4_scale(&scale_m, v3f(0.2,0.2,1));
    ras_m4_translate(&tr_m, v3f(((float)mouse.x/fmbuff->size.x)*2.f - 1., ((float)mouse.y/fmbuff->size.y)*2.0 - 1.0, 0));

    cmd.transform = tr_m * scale_m * rot_m ;

    cmd.triangle_cmd.draw_mode = ras_triangle_draw_mode_bary;
    //---clockwise test---
    tri = cmd.triangles + 4;
    tri->position[0] = v4f(0,1.,0,1);
    tri->position[2] = v4f(1.,1.,0,1);
    tri->position[1] = v4f(0,1.,0,1);

    ras_fill_framebuffer(fmbuff, v3ui8(255,255,0));
    ras_draw_triangle_list(fmbuff, &cmd); 
    ras_free(cmd.triangles);
}


void set_face(ras_triangle_list_cmd_t& cmd, int& k, const m4f& model) {
#define def_tri(i, j, a, b, c) \
    cmd.triangles[i].position[j]=v4f(a,b,c,1);\
    tmp = v4f(cmd.triangles[i].position[j].x, cmd.triangles[i].position[j].y, cmd.triangles[i].position[j].z, 1);\
    tmp = model * tmp;\
    cmd.triangles[i].position[j]=v4f(tmp.x,tmp.y,tmp.z,1);\
    radius = ras_sqrt(ras_pow(cmd.triangles[i].position[j].x,2.) + ras_pow(cmd.triangles[i].position[j].y,2.) + ras_pow(cmd.triangles[i].position[j].z,2.));\
    cmd.triangles[i].position[j].x /= radius; cmd.triangles[i].position[j].y /= radius;\
    cmd.triangles[i].position[j].z /= radius;

    float s = 0.2;
    v4f tmp;
    double radius = 1.0;
    for (float j = -1.0; j < 0.9; j += s) {
        for (float i = -1.0; i < 0.9; i+= s) {
            def_tri(k+0,0, i,    j,-1);
            def_tri(k+0,1, i+s,j,-1);
            def_tri(k+0,2, i,    j+s,-1);

            def_tri(k+1,0, i+s, j,-1);
            def_tri(k+1,1, i+s, j+s,-1);
            def_tri(k+1,2, i,   j+s,-1);
            k += 2;
        }
    }
#undef def_tri
}

void test_subdiv_sphere() {
    ras_framebuffer_t* fmbuff = ras_get_main_framebuffer();
    ras_triangle_list_cmd_t cmd{};
    cmd.count     = 2*10*10*6;
    cmd.triangles = ras_alloc_n(ras_prim_triangle_t, cmd.count);
    int k = 0;

    m4f   proj_m(1.f); 
    m4f   model_m(1.f);
    m4f   view_m(1.f);
    m4f   rot_m(1.f); 
    m4f   rot_m2(1.f); 
    m4f   scale_m(1.f);
    m4f   tr_m(1.f);

    //front
    set_face(cmd, k, model_m);
    //right
    ras_set_m4_diag(&model_m,1.0f);
    ras_m4_set_rot_y(&model_m, -3.1415 * 0.5);
    //ras_m4_translate(&model_m, v3f(2.0,0.,0.));
    set_face(cmd, k , model_m);
    //back
    ras_set_m4_diag(&model_m,1.0f);
    ras_m4_set_rot_x(&model_m, 3.1415);
    ras_m4_translate(&model_m, v3f(0.0,0.,0.));
    set_face(cmd, k , model_m);
    //left
    ras_set_m4_diag(&model_m,1.0f);
    ras_m4_set_rot_y(&model_m, 3.1415 *0.5);
    //ras_m4_translate(&model_m, v3f(-2.0,0.,0.));
    set_face(cmd, k , model_m);
    //top
    ras_set_m4_diag(&model_m,1.0f);
    ras_m4_set_rot_x(&model_m, 3.1415 * 0.5);
    set_face(cmd, k , model_m);
    //buttom
    ras_set_m4_diag(&model_m,1.0f);
    ras_m4_set_rot_x(&model_m, -3.1415 * 0.5);
    set_face(cmd, k , model_m);

    ras_set_m4_diag(&cmd.transform, 1.0f);
    ras_set_m4_diag(&rot_m,   1.0f);
    ras_set_m4_diag(&rot_m2,   1.0f);
    ras_set_m4_diag(&scale_m, 1.0f);
    ras_set_m4_diag(&view_m, 1.0f);
    ras_set_m4_diag(&model_m, 1.0f);

    static float x = 0;
    x += -0.01;

    ras_m4_set_rot_x(&rot_m,  x);
    ras_m4_set_rot_y(&rot_m2, x);
    ras_m4_scale(&scale_m, v3f(1,1,1));
    ras_m4_translate(&tr_m, v3f(0, 0, 2));
    ras_m4_perspective(&proj_m, 70.0f * 3.1415f / 180.0f, (float)fmbuff->size.y / fmbuff->size.x, .0f, 124.0f);
    
    ras_m4_translate(&tr_m, v3f(0.0));

    ras_m4_translate(&view_m, v3f(0, 0, 0.0));

    rot_m   = rot_m2 * rot_m;
    model_m = tr_m * scale_m * rot_m;
    cmd.transform = proj_m * view_m * model_m;
    cmd.triangle_cmd.draw_mode = ras_triangle_draw_mode_wireframe;
    cmd.cull_mode = ras_orientation_cw;
    cmd.triangle_cmd.wireframe_width = 0.05;
    ras_fill_framebuffer(fmbuff, v3ui8(25,25,25));
    ras_draw_triangle_list(fmbuff, &cmd); 
    ras_free(cmd.triangles);
}

void test_draw_cube2() {
    ras_framebuffer_t* fmbuff = ras_get_main_framebuffer();
    ras_triangle_list_cmd_t cmd{};
    cmd.count     = 12 ;
    cmd.triangles = ras_alloc_n(ras_prim_triangle_t, cmd.count);
    cmd.tris_data.data         = ras_alloc_n(float, 12 * 9);
    cmd.tris_data.stride_count = 3;

#define def_tri(i, j, a, b, c,col) \
    cmd.triangles[i].position[j]=v4f(a,b,c,1); \
    *(cmd.tris_data.data + (i * 9 + 0 +3*j)) = col.x;\
    *(cmd.tris_data.data + (i * 9 + 1 +3*j)) = col.y;\
    *(cmd.tris_data.data + (i * 9 + 2 +3*j)) = col.z;\

    def_tri(0,0,-1,-1,-1,v3ui8(255,0,0));
    def_tri(0,1, 1,-1,-1,v3ui8(255,0,0));
    def_tri(0,2,-1, 1,-1,v3ui8(255,0,0));
    def_tri(1,0, 1,-1,-1,v3ui8(255,0,0));
    def_tri(1,1, 1, 1,-1,v3ui8(255,0,0));
    def_tri(1,2,-1, 1,-1,v3ui8(255,0,0));

    def_tri(2,0, 1,-1,1,v3ui8(0,255,0));
    def_tri(2,1,-1,-1,1,v3ui8(0,255,0));
    def_tri(2,2,-1, 1,1,v3ui8(0,255,0));
    def_tri(3,0, 1, 1,1,v3ui8(0,255,0));
    def_tri(3,1, 1,-1,1,v3ui8(0,255,0));
    def_tri(3,2,-1, 1,1,v3ui8(0,255,0));

    def_tri(4,0,1,-1,-1,v3ui8(0,0,255));
    def_tri(4,1,1,-1, 1,v3ui8(0,0,255));
    def_tri(4,2,1, 1,-1,v3ui8(0,0,255));
    def_tri(5,0,1,-1, 1,v3ui8(0,0,255));
    def_tri(5,1,1, 1, 1,v3ui8(0,0,255));
    def_tri(5,2,1, 1,-1,v3ui8(0,0,255));

    def_tri(6,0,-1,-1, 1,v3ui8(255,255,0));
    def_tri(6,1,-1,-1,-1,v3ui8(255,255,0));
    def_tri(6,2,-1, 1,-1,v3ui8(255,255,0));
    def_tri(7,0,-1, 1, 1,v3ui8(255,255,0));
    def_tri(7,1,-1,-1, 1,v3ui8(255,255,0));
    def_tri(7,2,-1, 1,-1,v3ui8(255,255,0));

    def_tri(8,0,-1,1,-1,v3ui8(0,255,255));
    def_tri(8,1, 1,1,-1,v3ui8(0,255,255));
    def_tri(8,2,-1,1, 1,v3ui8(0,255,255));
    def_tri(9,0, 1,1,-1,v3ui8(0,255,255));
    def_tri(9,1, 1,1, 1,v3ui8(0,255,255));
    def_tri(9,2,-1,1, 1,v3ui8(0,255,255));

    def_tri(10,0,-1, -1,1,v3ui8(255,0,255));
    def_tri(10,1,1,  -1,1,v3ui8(255,0,255));
    def_tri(10,2,-1, -1,-1,v3ui8(255,0,255));
    def_tri(11,0,-1, -1,-1,v3ui8(255,0,255));
    def_tri(11,1, 1, -1,1,v3ui8(255,0,255));
    def_tri(11,2, 1, -1,-1,v3ui8(255,0,255));
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
    ras_m4_scale(&scale_m,  v3f(1,1,1));
    ras_m4_translate(&tr_m, v3f(0, 0, 0));
    ras_m4_perspective(&proj_m, 90.0f * 3.1415f / 180.0f, (float)fmbuff->size.y / fmbuff->size.x, 0.1f, 64.0f);
    
    ras_m4_translate(&tr_m, v3f(0.0));

    ras_m4_translate(&view_m, v3f(0, 0, 0));

    rot_m   = rot_m2 * rot_m;
    model_m = tr_m * scale_m * rot_m;
    cmd.transform = proj_m * view_m * model_m;
    cmd.triangle_cmd.draw_mode = ras_triangle_draw_mode_user_func;
    cmd.cull_mode = ras_orientation_none;
    ras_register_frag_program(interpolate_v3_prog, 1);
    cmd.triangle_cmd.frag_prg = 1;
    ras_fill_framebuffer(fmbuff, v3ui8(25,25,25));
    ras_draw_triangle_list(fmbuff, &cmd); 
    ras_free(cmd.tris_data.data);
    ras_free(cmd.triangles);
}
void test_draw_cube() {
    ras_framebuffer_t* fmbuff = ras_get_main_framebuffer();
    ras_triangle_list_cmd_t cmd{};
    cmd.count     = 12 ;
    cmd.triangles = ras_alloc_n(ras_prim_triangle_t, cmd.count);

#define def_tri(i, j, x, y, z) \
    cmd.triangles[i].position[j]=v4f(x,y,z,1);

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
    ras_m4_perspective(&proj_m, 90.0f * 3.1415f / 180.0f, (float)fmbuff->size.y / fmbuff->size.x, 1.0f, 124.0f);
    
    ras_m4_translate(&tr_m, v3f(0.0));

    ras_m4_translate(&view_m, v3f(0, 0, 10));

    rot_m   = rot_m2 * rot_m;
    model_m = tr_m * scale_m * rot_m;
    cmd.transform = proj_m * view_m * model_m;
    cmd.triangle_cmd.draw_mode = ras_triangle_draw_mode_wireframe;
    cmd.cull_mode = ras_orientation_cw;
    ras_fill_framebuffer(fmbuff, v3ui8(25,25,25));
    ras_draw_triangle_list(fmbuff, &cmd); 
    ras_free(cmd.triangles);
}

void test_clipping() {
    ras_framebuffer_t* fmbuff = ras_get_main_framebuffer();
    ras_triangle_list_cmd_t cmd{};
    cmd.count     = 1;
    cmd.triangles = ras_alloc_n(ras_prim_triangle_t, cmd.count);

#define def_tri(j, x, y) \
    cmd.triangles[0].position[j]=v4f(x,y,0,1);
    def_tri(0, 1,-1);
    def_tri(1,-1, 1);
    def_tri(2,-1,-1);
#undef def_tri
    m4f   proj_m; 
    m4f   model_m;
    m4f   view_m;
    m4f   rot_m; 
    m4f   rot_m2; 
    m4f   scale_m;
    m4f   tr_m;
    ras_set_m4_diag(&cmd.transform, 1.0f);
    ras_set_m4_diag(&rot_m,         1.0f);
    ras_set_m4_diag(&rot_m2,        1.0f);
    ras_set_m4_diag(&scale_m,       1.0f);
    ras_set_m4_diag(&tr_m,          1.0f);
    ras_set_m4_diag(&proj_m,        1.0f);
    ras_set_m4_diag(&view_m,        1.0f);
    ras_set_m4_diag(&model_m,       1.0f);

    static float x = 0;
    //x += 0.1;

    ras_m4_scale(&scale_m, v3f(10.0,10.0,1));
    ras_m4_translate(&tr_m, v3f(0.,0, 0));
    ras_m4_perspective(&proj_m, 90.0f * 3.1415f / 180.0f, (float)fmbuff->size.y / fmbuff->size.x, 1.0f, 124.0f);
    
    ras_m4_translate(&tr_m, v3f(0.0));

    ras_m4_translate(&view_m, v3f(0, 0, 1));

    rot_m   = rot_m2 * rot_m;
    model_m = tr_m * scale_m * rot_m;
    cmd.transform = proj_m * view_m * model_m;
    cmd.cull_mode = ras_orientation_none;
    cmd.triangle_cmd.wireframe_width = 0.01;
    cmd.triangle_cmd.draw_mode = ras_triangle_draw_mode_wireframe;
    ras_fill_framebuffer(fmbuff, v3ui8(25,25,25));
    ras_draw_triangle_list(fmbuff, &cmd); 
    ras_free(cmd.triangles);
}

void test_draw_plane() {
    ras_framebuffer_t* fmbuff = ras_get_main_framebuffer();
    ras_triangle_list_cmd_t cmd{};
    cmd.count     = 2;
    cmd.triangles = ras_alloc_n(ras_prim_triangle_t, cmd.count);

    cmd.triangles[1].position[0] = v4f(-1,-1,0,1);
    cmd.triangles[1].position[1] = v4f(1,-1,0,1);
    cmd.triangles[1].position[2] = v4f(-1,1,0,1);

    cmd.triangles[0].position[0] = v4f(-1,1,0,1);
    cmd.triangles[0].position[1] = v4f(1,-1,0,1);
    cmd.triangles[0].position[2] = v4f(1,1,0,1);

    float colors[] = {
        255.0, 0., 0.,
        0.0,   255,0.,
        0.0,   0., 255.,

        0.0,   0., 255.,
        0.0,   255,0.,
        255.0, 0., 0.,
    };
    cmd.tris_data.stride_count = 3;
    cmd.tris_data.data         = colors;
    m4f   proj_m (1.0f); 
    m4f   model_m(1.0f);
    m4f   view_m (1.0f);
    m4f   rot_m  (1.0f); 
    m4f   scale_m(1.0f);
    m4f   tr_m   (1.0f);

    static float x = 0;
    x += 0.02;
    ras_m4_set_rot_x(&rot_m, 0);
    ras_m4_scale(&scale_m, v3i32(10,10,1));
    ras_m4_perspective(&proj_m, 70.0f * 3.1415f / 180.0f, (float)fmbuff->size.y / fmbuff->size.x, 1.0f, 124.0f);
    
    ras_m4_translate(&tr_m, v3f(0,x));

    ras_m4_translate(&view_m, v3f(0, 0, 20));

    model_m = tr_m * scale_m * rot_m;
    cmd.transform = proj_m * view_m * model_m;
    cmd.triangle_cmd.draw_mode = ras_triangle_draw_mode_user_func;
    cmd.cull_mode              = ras_orientation_cw;
    cmd.triangle_cmd.frag_prg  = 1;
    ras_register_frag_program(interpolate_v3_prog,1);

    ras_fill_framebuffer(fmbuff, v3ui8(10,10,20));
    ras_draw_triangle_list(fmbuff, &cmd); 
    ras_free(cmd.triangles);
}

void test_textured_plane() {
    ras_framebuffer_t* fmbuff = ras_get_main_framebuffer();
    ras_triangle_list_cmd_t cmd{};
    cmd.count     = 2;
    cmd.triangles = ras_alloc_n(ras_prim_triangle_t, cmd.count);

    cmd.triangles[0].position[0] = v4f(-1,-1,0,1);
    cmd.triangles[0].position[1] = v4f(1,-1,0,1);
    cmd.triangles[0].position[2] = v4f(-1,1,0,1);

    cmd.triangles[1].position[0] = v4f(-1,1,0,1);
    cmd.triangles[1].position[1] = v4f(1,-1,0,1);
    cmd.triangles[1].position[2] = v4f(1,1,0,1);

    float coords[] = {
        0.0,   0.0,
        1.0,   0.0,
        0.0,   1.0,

        0.0,   1.0,
        1.0,   0.0,
        1.0,   1.0,
    };
    cmd.tris_data.stride_count = 2;
    cmd.tris_data.data         = coords;
    m4f   proj_m (1.0f), model_m(1.0f), view_m (1.0f);
    m4f   rot_m  (1.0f); 
    m4f   scale_m(1.0f);
    m4f   tr_m   (1.0f);
    static float x = 0;
    x += 0.01;
    ras_m4_set_rot_x(&rot_m, x);
    ras_m4_scale(&scale_m, v3i32(10,10,1));
    ras_m4_perspective(&proj_m, 70.0f * 3.1415f / 180.0f, (float)fmbuff->size.y / fmbuff->size.x, 1.0f, 124.0f);
    ras_m4_translate(&tr_m, v3f(0,0));
    ras_m4_translate(&view_m, v3f(0, 0, 17));

    model_m = tr_m * scale_m * rot_m;
    cmd.transform = proj_m * view_m * model_m;
    cmd.triangle_cmd.draw_mode = ras_triangle_draw_mode_user_func;
    cmd.cull_mode              = ras_orientation_none;
    cmd.triangle_cmd.frag_prg  = 2;
    ras_register_frag_program(texture_prog,2);
    ras_fill_framebuffer(fmbuff, v3ui8(10,10,20));
    ras_draw_triangle_list(fmbuff, &cmd); 
    ras_free(cmd.triangles);
}
