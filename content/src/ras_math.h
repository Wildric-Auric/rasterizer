#ifndef RAS_MATH
#define RAS_MATH

typedef unsigned char  ui8;
typedef char           i8;
typedef unsigned short ui16;
typedef short          i16;
typedef unsigned int   ui32;
typedef int            i32;

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


#define def_vec3(type, attrib_type) struct type { \
    attrib_type x; attrib_type y; attrib_type z; \
    type(attrib_type xp = 0, attrib_type yp = 0, attrib_type zp = 0) { x = xp; y = yp; z = zp;} \
    type(const type& other) { x = other.x; y = other.y; z = other.z;} \
    def_op1_v3(type, +); def_op1_v3(type, -);\
    def_op1_v3(type, *); def_op1_v3(type, /);\
    def_op2_v3(type, +); def_op2_v3(type, -);\
    def_op2_v3(type, *); def_op2_v3(type, /);\
    def_unary_op_v3(type);\
}

#define def_vec2(type, attrib_type) struct type { \
    attrib_type x; attrib_type y; \
    type(attrib_type xp = 0, attrib_type yp = 0) { x = xp; y = yp; } \
    type(const type& other) { x = other.x; y = other.y; } \
    def_op1_v2(type, +); def_op1_v2(type, -);\
    def_op1_v2(type, *); def_op1_v2(type, /);\
    def_op2_v2(type, +); def_op2_v2(type, -);\
    def_op2_v2(type, *); def_op2_v2(type, /);\
    def_unary_op_v2(type);\
}

#define def_vec4(type, attrib_type) struct type { \
    attrib_type x; attrib_type y; attrib_type z; attrib_type w; \
    type(attrib_type xp = 0, attrib_type yp = 0, attrib_type zp = 0, attrib_type wp = 0) { x = xp; y = yp; z = zp; w = wp;} \
    type(const type& other) { x = other.x; y = other.y; z = other.z; w = other.w;} \
    def_op1_v4(type, +); def_op1_v4(type, -);\
    def_op1_v4(type, *); def_op1_v4(type, /);\
    def_op2_v4(type, +); def_op2_v4(type, -);\
    def_op2_v4(type, *); def_op2_v4(type, /);\
    def_unary_op_v2(type);\
}

def_vec3(v3i8, i8);
def_vec3(v3ui8, ui8);
def_vec3(v3i32 , i32);
def_vec3(v3f  , float);

def_vec2(v2i8 , i8);
def_vec2(v2i32, i32);
def_vec2(v2f  , float);

def_vec4(v4i8  , i8);
def_vec4(v4ui8 , ui8);
def_vec4(v4i32 , i32);
def_vec4(v4f   , float);


#define ras_min(a,b)       (a) < (b) ? (a) : (b)
#define ras_max(a,b)       (a) > (b) ? (a) : (b)
#define ras_clamp(a,mi,ma) ras_max(mi,ras_min(a, ma))
#define ras_min3(a,b,c)    ras_min(ras_min((a),(b)),(c))
#define ras_max3(a,b,c)    ras_max(ras_max((a),(b)),(c))

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


int ras_det2(const v2i32&, const v2i32&);

#endif
