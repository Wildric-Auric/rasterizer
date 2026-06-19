#ifndef RAS_TEST_H
#define RAS_TEST_H
#include "ras_math.h"

void texture_prog(const v3f* const bary, float* const v_data[3], v3f* const out_color);
void interpolate_v3_prog(const v3f* const bary, float* const v_data[3], v3f* const out_color);

void test_draw_circle();
void test_draw_triangle();
void test_draw_triangles();
void test_draw_plane();
void test_draw_cube();
void test_subdiv_sphere();
void test_clipping();
void test_draw_cube2();
void test_textured_plane();

#endif
