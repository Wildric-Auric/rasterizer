#ifndef RAS_MATH
#define RAS_MATH

typedef unsigned char  ui8;
typedef char           i8;
typedef unsigned short ui16;
typedef short          i16;
typedef unsigned int   ui32;
typedef int            i32;

// -------- General functions --------

double ras_cos(double);
double ras_sin(double);
double ras_tan(double);
double ras_sqrt(double);
double ras_pow(double, double);

// -------- Vectors --------
#define def_unary_op_v3(type) type operator-() const { return type(-x,-y,-z);}
#define def_op1_v3(type, op) type operator op (const type& other) const {return type(x op other.x, y op other.y, z op other.z);} 
#define def_op2_v3(type, op) void operator op##=(const type& other) { x op##= other.x, y op##= other.y, z op##= other.z;} 

#define def_unary_op_v2(type) type operator-() const { return type(-x,-y);}
#define def_op1_v2(type, op) type operator op (const type& other) const {return type(x op other.x, y op other.y);} 
#define def_op2_v2(type, op) void operator op##=(const type& other) {x op##= other.x; y op##= other.y;}

#define def_unary_op_v4(type) type operator-() { return type(-x,-y,-z,-w);}
#define def_op1_v4(type, op) type operator op (const type& other)   \
{ return type(x op other.x, y op other.y, z op other.z, w op other.w);} 
#define def_op2_v4(type, op) void operator op##=(const type& other) \
{x op##= other.x; y op##= other.y; z op##= other.z; w op##= other.w;} 

template<typename scalar_type_g>
struct ras_vec3 {
    scalar_type_g x; scalar_type_g y; scalar_type_g z;
    explicit ras_vec3<scalar_type_g>(scalar_type_g xp = 0, scalar_type_g yp = 0, scalar_type_g zp = 0) { x = xp; y = yp; z = zp;}
    ras_vec3<scalar_type_g>(const ras_vec3<scalar_type_g>& other) { x = other.x; y = other.y; z = other.z;}
    bool operator == (const ras_vec3<scalar_type_g>& other) { return x == other.x && y == other.y && z == other.z;}
    def_op1_v3(ras_vec3<scalar_type_g>, +); def_op1_v3(ras_vec3<scalar_type_g>, -);
    def_op1_v3(ras_vec3<scalar_type_g>, *); def_op1_v3(ras_vec3<scalar_type_g>, /);
    def_op2_v3(ras_vec3<scalar_type_g>, +); def_op2_v3(ras_vec3<scalar_type_g>, -);
    def_op2_v3(ras_vec3<scalar_type_g>, *); def_op2_v3(ras_vec3<scalar_type_g>, /);
    def_unary_op_v3(ras_vec3<scalar_type_g>);
};


template<typename scalar_type_g>
struct ras_vec2 {
    scalar_type_g x; scalar_type_g y; 
    explicit ras_vec2<scalar_type_g>(scalar_type_g xp = 0, scalar_type_g yp = 0) { x = xp; y = yp; }
    ras_vec2<scalar_type_g>(const ras_vec2<scalar_type_g>& other) { x = other.x; y = other.y; } 
    bool operator == (const ras_vec2<scalar_type_g>& other) { return x == other.x && y == other.y;}
    def_op1_v2(ras_vec2<scalar_type_g>, +); def_op1_v2(ras_vec2<scalar_type_g>, -);
    def_op1_v2(ras_vec2<scalar_type_g>, *); def_op1_v2(ras_vec2<scalar_type_g>, /);
    def_op2_v2(ras_vec2<scalar_type_g>, +); def_op2_v2(ras_vec2<scalar_type_g>, -);
    def_op2_v2(ras_vec2<scalar_type_g>, *); def_op2_v2(ras_vec2<scalar_type_g>, /);
    def_unary_op_v2(ras_vec2<scalar_type_g>);
};

template<typename scalar_type_g>
struct ras_vec4 {
    scalar_type_g x; scalar_type_g y; scalar_type_g z; scalar_type_g w;
    explicit ras_vec4<scalar_type_g>(scalar_type_g xp = 0, scalar_type_g yp = 0, scalar_type_g zp = 0, scalar_type_g wp = 0) { x = xp; y = yp; z = zp; w = wp;}
    ras_vec4<scalar_type_g>(const ras_vec4<scalar_type_g>& other) { x = other.x; y = other.y; z = other.z; w = other.w;}
    bool operator == (const ras_vec4<scalar_type_g>& other) { return x == other.x && y == other.y && z == other.z && w == other.w;}
    def_op1_v4(ras_vec4<scalar_type_g>, +); def_op1_v4(ras_vec4<scalar_type_g>, -);
    def_op1_v4(ras_vec4<scalar_type_g>, *); def_op1_v4(ras_vec4<scalar_type_g>, /);
    def_op2_v4(ras_vec4<scalar_type_g>, +); def_op2_v4(ras_vec4<scalar_type_g>, -);
    def_op2_v4(ras_vec4<scalar_type_g>, *); def_op2_v4(ras_vec4<scalar_type_g>, /);
    def_unary_op_v2(ras_vec4<scalar_type_g>);
};

#define scalar_op_v4(op)\
template<typename scal_type_g>\
inline ras_vec4<scal_type_g> operator op(const scal_type_g scalar, const ras_vec4<scal_type_g>& v) {\
    return ras_vec4<scal_type_g>(scalar op v.x, scalar op v.y, scalar op v.z, scalar op v.w);\
}

#define scalar_op2_v4(op)\
template<typename scal_type_g>\
inline ras_vec4<scal_type_g> operator op(const ras_vec4<scal_type_g>& v, const scal_type_g scalar) {\
    return ras_vec4<scal_type_g>(v.x op scalar, v.y op scalar, v.z op scalar, v.w op scalar);\
}

#define scalar_op_v3(op)\
template<typename scal_type_g>\
inline ras_vec3<scal_type_g> operator op (const scal_type_g scalar, const ras_vec3<scal_type_g>& v) {\
    return ras_vec3<scal_type_g>(scalar op v.x, scalar op v.y, scalar op v.z);\
}

#define scalar_op2_v3(op)\
template<typename scal_type_g>\
inline ras_vec3<scal_type_g> operator op (const ras_vec3<scal_type_g>& v, const scal_type_g scalar) {\
    return ras_vec3<scal_type_g>(v.x op scalar, v.y op scalar, v.z op scalar);\
}

#define scalar_op_v2(op)\
template<typename scal_type_g>\
inline ras_vec2<scal_type_g> operator op (const scal_type_g scalar, const ras_vec2<scal_type_g>& v) {\
    return ras_vec2<scal_type_g>(scalar op v.x, scalar op v.y);\
}
#define scalar_op2_v2(op)\
template<typename scal_type_g>\
inline ras_vec2<scal_type_g> operator op (const ras_vec2<scal_type_g>& v, const scal_type_g scalar) {\
    return ras_vec2<scal_type_g>(v.x op scalar, v.y op scalar);\
}

scalar_op_v2(+)
scalar_op_v2(-)
scalar_op_v2(*)

scalar_op_v3(+)
scalar_op_v3(-)
scalar_op_v3(*)

scalar_op_v4(+)
scalar_op_v4(-)
scalar_op_v4(*)

scalar_op2_v2(*)
scalar_op2_v2(/)

scalar_op2_v3(/)
scalar_op2_v3(*)

scalar_op2_v4(/)
scalar_op2_v4(*)


#ifdef RAS_MATH_IMPL
#define tmpl_prefix 
#else
#define tmpl_prefix extern
#endif


tmpl_prefix template class ras_vec3<i8>;
tmpl_prefix template class ras_vec3<ui8>;
tmpl_prefix template class ras_vec3<i32>;
tmpl_prefix template class ras_vec3<float>;

tmpl_prefix template class ras_vec2<i8>;
tmpl_prefix template class ras_vec2<i32>;
tmpl_prefix template class ras_vec2<float>;

tmpl_prefix template class ras_vec4<i8>;
tmpl_prefix template class ras_vec4<ui8>;
tmpl_prefix template class ras_vec4<i32>;
tmpl_prefix template class ras_vec4<float>;

#undef def_op1_v2
#undef def_op2_v2
#undef def_op1_v3
#undef def_op2_v3
#undef def_op1_v4
#undef def_op2_v4

#undef def_vec2
#undef def_vec3
#undef def_vec4

#undef def_unary_op_v2
#undef def_unary_op_v3
#undef def_unary_op_v4

// -------- Matrices --------

#define def_op_mat4(type,op) \
ras_mat4<type> operator op (const ras_mat4<type>& other) const { \
    ras_mat4<type> ret;\
    ret.values[0] = values[0] op other.values[0];\
    ret.values[1] = values[1] op other.values[1];\
    ret.values[2] = values[2] op other.values[2];\
    ret.values[3] = values[3] op other.values[3];\
    ret.values[4] = values[4] op other.values[4];\
    ret.values[5] = values[5] op other.values[5];\
    ret.values[6] = values[6] op other.values[6];\
    ret.values[7] = values[7] op other.values[7];\
    ret.values[8] = values[8] op other.values[8];\
    ret.values[9] = values[9] op other.values[9];\
    ret.values[10] = values[10] op other.values[10];\
    ret.values[11] = values[11] op other.values[11];\
    ret.values[12] = values[12] op other.values[12];\
    ret.values[13] = values[13] op other.values[13];\
    ret.values[14] = values[14] op other.values[14];\
    ret.values[15] = values[15] op other.values[15];\
    return ret;\
}
 

template<typename scal_type_g>
struct ras_mat4 {
    scal_type_g values[16];
    ras_mat4(scal_type_g val = 0) {
        values[0]  = val;
        values[1]  = 0;
        values[2]  = 0;
        values[3]  = 0;
        values[4]  = 0;
        values[5]  = val;
        values[6]  = 0;
        values[7]  = 0;
        values[8]  = 0;
        values[9]  = 0;
        values[10] = val;
        values[11] = 0;
        values[12] = 0;
        values[13] = 0;
        values[14] = 0;
        values[15] = val;
    }
    inline scal_type_g* operator[](int i) {return values + 4 * i;}
    inline const scal_type_g*  operator[](int i) const {return values + 4 * i;}
    def_op_mat4(scal_type_g, +)
    def_op_mat4(scal_type_g, -)
    template<typename scal_type_other_g> 
    ras_mat4(const ras_mat4<scal_type_other_g> m) {
        this->values[0]  = m.values[0]; this->values[1] = m.values[1]; this->values[2] = m.values[2];
        this->values[3]  = m.values[3]; this->values[4] = m.values[4]; this->values[5] = m.values[5];
        this->values[6]  = m.values[6]; this->values[7] = m.values[7]; this->values[8] = m.values[8];
        this->values[9]  = m.values[9]; this->values[10] = m.values[10]; this->values[11] = m.values[11];
        this->values[12] = m.values[12]; this->values[13] = m.values[13]; this->values[14] = m.values[14];
        this->values[15] = m.values[15];
    }
};

template<typename scal_type_left_g, typename scal_type_right_g>
ras_mat4<scal_type_left_g> operator*(const ras_mat4<scal_type_left_g>& left, const ras_mat4<scal_type_right_g>& other) {
    ras_mat4<scal_type_left_g> res;
    res[0][0] = left[0][0] * other[0][0] + left[0][1] * other[1][0] + left[0][2] * other[2][0] + left[0][3] * other[3][0];
    res[0][1] = left[0][0] * other[0][1] + left[0][1] * other[1][1] + left[0][2] * other[2][1] + left[0][3] * other[3][1];
    res[0][2] = left[0][0] * other[0][2] + left[0][1] * other[1][2] + left[0][2] * other[2][2] + left[0][3] * other[3][2];
    res[0][3] = left[0][0] * other[0][3] + left[0][1] * other[1][3] + left[0][2] * other[2][3] + left[0][3] * other[3][3];

    res[1][0] = left[1][0] * other[0][0] + left[1][1] * other[1][0] + left[1][2] * other[2][0] + left[1][3] * other[3][0];
    res[1][1] = left[1][0] * other[0][1] + left[1][1] * other[1][1] + left[1][2] * other[2][1] + left[1][3] * other[3][1];
    res[1][2] = left[1][0] * other[0][2] + left[1][1] * other[1][2] + left[1][2] * other[2][2] + left[1][3] * other[3][2];
    res[1][3] = left[1][0] * other[0][3] + left[1][1] * other[1][3] + left[1][2] * other[2][3] + left[1][3] * other[3][3];

    res[2][0] = left[2][0] * other[0][0] + left[2][1] * other[1][0] + left[2][2] * other[2][0] + left[2][3] * other[3][0];
    res[2][1] = left[2][0] * other[0][1] + left[2][1] * other[1][1] + left[2][2] * other[2][1] + left[2][3] * other[3][1];
    res[2][2] = left[2][0] * other[0][2] + left[2][1] * other[1][2] + left[2][2] * other[2][2] + left[2][3] * other[3][2];
    res[2][3] = left[2][0] * other[0][3] + left[2][1] * other[1][3] + left[2][2] * other[2][3] + left[2][3] * other[3][3];

    res[3][0] = left[3][0] * other[0][0] + left[3][1] * other[1][0] + left[3][2] * other[2][0] + left[3][3] * other[3][0];
    res[3][1] = left[3][0] * other[0][1] + left[3][1] * other[1][1] + left[3][2] * other[2][1] + left[3][3] * other[3][1];
    res[3][2] = left[3][0] * other[0][2] + left[3][1] * other[1][2] + left[3][2] * other[2][2] + left[3][3] * other[3][2];
    res[3][3] = left[3][0] * other[0][3] + left[3][1] * other[1][3] + left[3][2] * other[2][3] + left[3][3] * other[3][3];
    return res;
}

tmpl_prefix template class ras_mat4<float>;
tmpl_prefix template class ras_mat4<int>;

#undef def_op_mat4

template<typename mat_type_g, typename vec_type_g>
inline ras_vec4<vec_type_g> operator*(const ras_mat4<mat_type_g>& left, const ras_vec4<vec_type_g>& right) {
    ras_vec4<vec_type_g> res;
    res.x = left[0][0] * right.x + left[0][1] * right.y + left[0][2] * right.z + left[0][3] * right.w;
    res.y = left[1][0] * right.x + left[1][1] * right.y + left[1][2] * right.z + left[1][3] * right.w;
    res.z = left[2][0] * right.x + left[2][1] * right.y + left[2][2] * right.z + left[2][3] * right.w;
    res.w = left[3][0] * right.x + left[3][1] * right.y + left[3][2] * right.z + left[3][3] * right.w;
    return res;
}

template<typename scal_type_g> 
inline void ras_set_m4_diag(ras_mat4<scal_type_g>* m, scal_type_g val) {
    m->values[0]  = val;
    m->values[1]  = 0;
    m->values[2]  = 0;
    m->values[3]  = 0;
    m->values[4]  = 0;
    m->values[5]  = val;
    m->values[6]  = 0;
    m->values[7]  = 0;
    m->values[8]  = 0;
    m->values[9]  = 0;
    m->values[10] = val;
    m->values[11] = 0;
    m->values[12] = 0;
    m->values[13] = 0;
    m->values[14] = 0;
    m->values[15] = val;
}

template<typename scal_type_g>
inline void ras_set_m4_zero(ras_mat4<scal_type_g>* m) {
    m->values[0]  = 0;
    m->values[1]  = 0;
    m->values[2]  = 0;
    m->values[3]  = 0;
    m->values[4]  = 0;
    m->values[5]  = 0;
    m->values[6]  = 0;
    m->values[7]  = 0;
    m->values[8]  = 0;
    m->values[9]  = 0;
    m->values[10] = 0;
    m->values[11] = 0;
    m->values[12] = 0;
    m->values[13] = 0;
    m->values[14] = 0;
    m->values[15] = 0;
}

template<typename scal_type_g, typename scal_type_vec_g> 
void ras_m4_translate(ras_mat4<scal_type_g>* m, const ras_vec3<scal_type_vec_g>& v) {
    m->values[3]  += v.x;
    m->values[7]  += v.y;
    m->values[11] += v.z;
}
template<typename scal_type_g, typename scal_type_vec_g> 
void ras_m4_scale(ras_mat4<scal_type_g>* m, const ras_vec3<scal_type_vec_g>& v) {
    m->values[0]  *= v.x;
    m->values[5]  *= v.y;
    m->values[10] *= v.z;
}

template<typename scal_type_g>
void ras_m4_set_rot_x(ras_mat4<scal_type_g>* m, double angle) {
    double c = ras_cos(angle);
    double s = ras_sin(angle);
    m->values[0]  =  1;
    m->values[5]  =  c;
    m->values[6]  = -s;
    m->values[9]  =  s;
    m->values[10] =  c;
}
template<typename scal_type_g>
void ras_m4_set_rot_y(ras_mat4<scal_type_g>* m, double angle) {
    double c = ras_cos(angle);
    double s = ras_sin(angle);
    m->values[0]  =  c;
    m->values[2]  =  s;
    m->values[8]  = -s;
    m->values[10] =  c;
}
template<typename scal_type_g>
void ras_m4_set_rot_z(ras_mat4<scal_type_g>* m, double angle) {
    double c = ras_cos(angle);
    double s = ras_sin(angle);
    m->values[0]  =  c;
    m->values[1]  = -s;
    m->values[4]  =  s;
    m->values[5]  =  c;
}
template<typename scal_type_g>
void ras_m4_perspective(ras_mat4<scal_type_g>* m, scal_type_g fov_y, scal_type_g aspect, scal_type_g near, scal_type_g far) {
    scal_type_g fmn = far - near;
    float cotn  = 1.0 / ras_tan(fov_y * 0.5);
    m->values[0]    = aspect * cotn;
    m->values[5]    = cotn;
    m->values[10]   = (far+near)/fmn;
    m->values[11]   = -2.0 * far * near / fmn;
    m->values[14]   =  1;
    m->values[15]   =  0;
}

template<typename scalar_g>
void ras_mat4_identity(ras_mat4<scalar_g>* m, scalar_g val) {
    m->values[0]  = val;
    m->values[1]  = 0; m->values[2] = 0; m->values[3] = 0; m->values[4] = 0;
    m->values[5]  = val;
    m->values[6]  = 0; m->values[7] = 0; m->values[8] = 0; m->values[9] = 0;
    m->values[10] = val;
    m->values[11] = 0; m->values[12] = 0; m->values[13] = 0; m->values[14] = 0;
    m->values[15] = val;
}
// -------- Define types -----

typedef ras_vec2<i8>    v2i8;
typedef ras_vec2<i32>   v2i32;
typedef ras_vec2<float> v2f;

typedef ras_vec3<i8>    v3i8;
typedef ras_vec3<ui8>   v3ui8;
typedef ras_vec3<i32>   v3i32;
typedef ras_vec3<float> v3f;

typedef ras_vec4<i8>    v4i8;
typedef ras_vec4<ui8>   v4ui8;
typedef ras_vec4<i32>   v4i32;
typedef ras_vec4<float> v4f;

typedef ras_mat4<float> m4f;
typedef ras_mat4<i32>   m4i32;
// -------- Functions --------


#define ras_min(a,b)       (a) < (b) ? (a) : (b)
#define ras_max(a,b)       (a) > (b) ? (a) : (b)
#define ras_clamp(a,mi,ma) ras_max((mi),ras_min((a), (ma)))
#define ras_min3(a,b,c)    ras_min(ras_min((a),(b)),(c))
#define ras_max3(a,b,c)    ras_max(ras_max((a),(b)),(c))


template<typename type>
type ras_det2(const ras_vec2<type>& v1, const ras_vec2<type>& v2) {
    return v1.x * v2.y - v1.y * v2.x;
}

float ras_v3f_dot(const v3f& v0, const v3f& v1);
float ras_v2f_dot(const v2f& v0, const v2f& v1);
float ras_v4f_dot(const v4f& v0, const v4f& v1);
float ras_v2f_norm(const v2f& v);
float ras_v3f_norm(const v3f& v);
float ras_v4f_norm(const v4f& v);
v4f ras_v4f_project(const v4f& u, const v4f& v);
v3f ras_v3f_project(const v3f& u, const v3f& v);
v2f ras_v2f_project(const v2f& u, const v2f& v);
v2f ras_to_v2f(const v4f&);
v3f ras_to_v3f(const v4f&);
v3f ras_cross_v3f(const v3f& u, const v3f);


#endif
