#include <model/vec3.h>
#include <math.h>

vec3_t vec3_vector_to(vec3_t a, vec3_t b) {
    return (vec3_t){a.x - b.x, a.y - b.y, a.z - b.z};
}

vec3_t vec3_normalize(vec3_t a) {
    float magnitude = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
    return (vec3_t){a.x / magnitude, a.y / magnitude, a.z / magnitude};
}

vec3_t vec3_add(vec3_t a, vec3_t b) {
    return (vec3_t){a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3_t vec3_subtract(vec3_t a, vec3_t b) {
    return (vec3_t){ a.x - b.x, a.y - b.y, a.z - b.z };
}

float vec3_distance_to(vec3_t a, vec3_t b) {
    float x = b.x - a.x;
    float y = b.y - a.y;
    float z = b.z - a.z;
    return sqrt(x * x + y * y + z * z);
}

float vec3_distance_to_sqr(vec3_t a, vec3_t b) {
    float x = b.x - a.x;
    float y = b.y - a.y;
    float z = b.z - a.z;
    return x * x + y * y + z * z;
}

float vec3_length(vec3_t a) {
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

vec3_t vec3_clip_x(vec3_t a, vec3_t b, float lerp) {
    vec3_t d = vec3_subtract(b, a);
    lerp = (lerp - a.x) / d.x;
    return d.x * d.x < 1.0E-7 ? vec3_null : (lerp >= 0.0 && lerp <= 1.0 ? (vec3_t){ a.x + d.x * lerp, a.y + d.y * lerp, a.z + d.z * lerp } : vec3_null);
}

vec3_t vec3_clip_y(vec3_t a, vec3_t b, float lerp) {
    vec3_t d = vec3_subtract(b, a);
    lerp = (lerp - a.y) / d.y;
    return d.y * d.y < 1.0E-7 ? vec3_null : (lerp >= 0.0 && lerp <= 1.0 ? (vec3_t){ a.x + d.x * lerp, a.y + d.y * lerp, a.z + d.z * lerp } : vec3_null);
}

vec3_t vec3_clip_z(vec3_t a, vec3_t b, float lerp) {
    vec3_t d = vec3_subtract(b, a);
    lerp = (lerp - a.z) / d.z;
    return d.z * d.z < 1.0E-7 ? vec3_null : (lerp >= 0.0 && lerp <= 1.0 ? (vec3_t){ a.x + d.x * lerp, a.y + d.y * lerp, a.z + d.z * lerp } : vec3_null);
}