#define RAS_MATH_IMPL
#include "ras_math.h"
#include <math.h>

double ras_cos(double x) {
    return cos(x);
}

double ras_sin(double x) {
    return sin(x);
}

double ras_tan(double x) {
    return tan(x);
}

double ras_sqrt(double x) {
    return sqrt(x);
}

double ras_pow(double x, double p) {
    return pow(x, p);
}

float ras_v3f_dot(const v3f& v0, const v3f& v1) {
    return v0.x*v1.x+v0.y*v1.y+v0.z*v1.z;
}

float ras_v2f_dot(const v2f& v0, const v2f& v1) {
    return v0.x*v1.x+v0.y*v1.y;
}

float ras_v4f_dot(const v4f& v0, const v4f& v1) {
    return v0.x*v1.x+v0.y*v1.y+v0.z*v1.z+v0.w*v1.w;
}

float ras_v2f_norm(const v2f& v) {
    return sqrt(pow(v.x, 2.) + pow(v.y, 2.));
}

float ras_v3f_norm(const v3f& v) {
    return sqrt(pow(v.x, 2.) + pow(v.y, 2.) + pow(v.z,2.));
}

float ras_v4f_norm(const v4f& v) {
    return sqrt(pow(v.x, 2.) + pow(v.y, 2.) + pow(v.z,2.) + pow(v.w, 2.));
}

v4f ras_v4f_project(const v4f& u, const v4f& v) {
    return ((float)(ras_v4f_dot(u, v)/pow(ras_v4f_norm(v),2.)))*v;
}

v3f ras_v3f_project(const v3f& u, const v3f& v) {
    return ((float)(ras_v3f_dot(u, v)/pow(ras_v3f_norm(v),2.)))*v;
}

v2f ras_v2f_project(const v2f& u, const v2f& v) {
    return ((float)(ras_v2f_dot(u, v)/pow(ras_v2f_norm(v),2.)))*v;
}

v2f ras_to_v2f(const v4f& f) {
    return v2f(f.x, f.y);
}

v3f ras_to_v3f(const v4f& v) {
    return v3f(v.x, v.y, v.z);
}

v3f ras_cross_v3f(const v3f& u, const v3f v) {
    return v3f(u.y * v.z - v.y * u.z, u.z * v.x - v.z * u.x, u.x * v.y - v.x * u.y);
}
