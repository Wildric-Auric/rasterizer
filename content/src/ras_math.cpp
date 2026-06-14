#include "ras_math.h"

int ras_det2(const v2i32& v1, const v2i32& v2) {
    return v1.x * v2.y - v1.y * v2.x;
}
