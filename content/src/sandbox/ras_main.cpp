#include "ras_core.h"
#include "ras_test.h"

// ----- loop -------

void ras_init() {
    ras_framebuffer_t* fmbuff = ras_get_main_framebuffer();
    ras_init_framebuffer(fmbuff, v2i32(1080,720));
    ras_backend_resize(fmbuff->size.x, fmbuff->size.y);
}

void ras_update() {
    //test_draw_plane();
    test_textured_plane();
    //test_subdiv_sphere();
    //test_draw_cube2();
    //test_draw_cube();
    //test_clipping();
    //test_draw_triangles();
    //test_draw_triangle();
}
