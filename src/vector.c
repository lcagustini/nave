inline static float vectorLen(struct vector v) {
    float len = v.x * v.x + v.y * v.y + v.z * v.z;
    return (float) fsqrt(len);
}

inline static void vectorNormalize(struct vector *v) {
    float len = v->x * v->x + v->y * v->y + v->z * v->z;
    if (!len)
        return;
    float div = frsqrt(len);
    v->x *= div;
    v->y *= div;
    v->z *= div;
}

inline static float vectorDot(struct vector a, struct vector b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline struct vector vectorCross(struct vector a, struct vector b) {
    struct vector n = {
        .x = a.y*b.z - a.z*b.y,
        .y = a.z*b.x - a.x*b.z,
        .z = a.x*b.y - a.y*b.x
    };
    return n;
}

inline struct vector vectorScale(float a, struct vector v) {
    v.x *= a;
    v.y *= a;
    v.z *= a;
    return v;
}

inline struct vector vectorAdd(struct vector a, struct vector b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

// https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
struct vector vectorRotate(struct vector v, struct quaternion q) {
    struct vector u = {.x = q.x, .y = q.y, .z = q.z};
    float s = q.w;

    return vectorAdd(vectorScale(2.0f * vectorDot(u, v), u),
            vectorAdd(vectorScale(s*s - vectorDot(u, u), v),
                vectorScale(2.0f * s, vectorCross(u, v))));
}

struct quaternion getRotationQuat(struct vector from, struct vector to) {
    struct quaternion result;
    struct vector H;

    H.x = from.x + to.x;
    H.y = from.y + to.y;
    H.z = from.z + to.z;
    vectorNormalize(&H);

    result.w = vectorDot(from, H);
    result.x = from.y*H.z - from.z*H.y;
    result.y = from.z*H.x - from.x*H.z;
    result.z = from.x*H.y - from.y*H.x;

    return result;
}

struct quaternion quatMult(struct quaternion a, struct quaternion b) {
    struct quaternion ret;

    ret.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
    ret.x = a.w*b.x + a.x*b.w - a.y*b.z + a.z*b.y;
    ret.y = a.w*b.y + a.x*b.z + a.y*b.w - a.z*b.x;
    ret.z = a.w*b.z - a.x*b.y + a.y*b.x + a.z*b.w;

    return ret;
}
