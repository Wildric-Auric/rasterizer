#include "ras_core.h"
#include "ras_test.h"
#include <stdio.h>

// ----- loop -------

void ras_init() {
    ras_framebuffer_t* fmbuff = ras_get_main_framebuffer();
    ras_init_framebuffer(fmbuff, v2i32(1080,720));
    ras_backend_resize(fmbuff->size.x, fmbuff->size.y);
}

void (*funcs[])(void) = {
    test_draw_plane,
    test_textured_plane,
    test_subdiv_sphere,
    test_draw_cube2,
    test_draw_cube2,
    test_draw_cube,
    test_clipping,
    test_draw_triangles,
    test_draw_triangle,
    test_depth,
};

int func_idx  = 0;
int funcs_num = sizeof(funcs) / sizeof(funcs[0]);

void ras_update() {
    int dx = ras_backend_get_key_on_release(ras_keys_arrow_up) - ras_backend_get_key_on_release(ras_keys_arrow_down);
    if (dx) {
        func_idx += dx;
        if (func_idx < 0)  
            func_idx = funcs_num - 1;
        if (func_idx == funcs_num)
            func_idx = 0;
        func_idx = ras_clamp(func_idx, 0, funcs_num - 1);
        printf("update: %d %d\n", func_idx, dx);
    }
    funcs[func_idx]();
}
