#include <physics/AABB.h>

static float epsilon = 0.0;

AABB_t AABB_expand(AABB_t a, float x, float y, float z) {
    a.x0 += x < 0.0 ? x : 0.0;
    a.y0 += y < 0.0 ? y : 0.0;
    a.z0 += z < 0.0 ? z : 0.0;
    a.x1 += x > 0.0 ? x : 0.0;
    a.y1 += y > 0.0 ? y : 0.0;
    a.z1 += z > 0.0 ? z : 0.0;
    return a;
}

AABB_t AABB_grow(AABB_t a, float x, float y, float z) {
    return (AABB_t){ a.x0 - x, a.y0 - y, a.z0 - z, a.x1 + x, a.y1 + y, a.z1 + z };
}

AABB_t AABB_move(AABB_t a, float x, float y, float z) {
    return (AABB_t){ a.x0 + x, a.y0 + y, a.z0 + z, a.x1 + x, a.y1 + y, a.z1 + z };
}

float AABB_get_size(AABB_t a) {
    float x = a.x1 - a.x0;
    float y = a.y1 - a.y0;
    float z = a.z1 - a.z0;
    return (x + y + z) / 3.0;
}

float AABB_clip_x_collide(AABB_t a, AABB_t b, float x) {
    if (b.y1 > a.y0 && b.y0 < a.y1) {
        if (b.z1 > a.z0 && b.z0 < a.z1) {
            float max = a.x0 - b.x1 - epsilon;
            if (x > 0.0 && b.x1 <= a.x0 && max < x) x = max;
            max = a.x1 - b.x0 + epsilon;
            if (x < 0.0 && b.x0 >= a.x1 && max > x) x = max;
            return x;
        } else {
            return x;
        }
    } else {
        return x;
    }
}

float AABB_clip_y_collide(AABB_t a, AABB_t b, float y) {
    if (b.x1 > a.x0 && b.x0 < a.x1) {
        if (b.z1 > a.z0 && b.z0 < a.z1) {
            float max = a.y0 - b.y1 - epsilon;
            if (y > 0.0 && b.y1 <= a.y0 && max < y) y = max;
            max = a.y1 - b.y0 + epsilon;
            if (y < 0.0 && b.y0 >= a.y1 && max > y) y = max;
            return y;
        } else {
            return y;
        }
    } else {
        return y;
    }
}

float AABB_clip_z_collide(AABB_t a, AABB_t b, float z) {
    if (b.x1 > a.x0 && b.x0 < a.x1) {
        if (b.y1 > a.y0 && b.y0 < a.y1) {
            float max = a.z0 - b.z1 - epsilon;
            if (z > 0.0 && b.z1 <= a.z0 && max < z) z = max;
            max = a.z1 - b.z0 + epsilon;
            if (z < 0.0 && b.z0 >= a.z1 && max > z) z = max;
            return z;
        } else {
            return z;
        }
    } else {
        return z;
    }
}

uint8_t AABB_intersects(AABB_t a, AABB_t b) {
    return b.x1 > a.x0 && b.x0 < a.x1 ? (b.y1 > a.y0 && b.y0 < a.y1 ? b.z1 > a.z0 && b.z0 < a.z1 : 0) : 0;
}

uint8_t AABB_intersects_inner(AABB_t a, AABB_t b) {
    return b.x1 >= a.x0 && b.x0 <= a.x1 ? (b.y1 >= a.y0 && b.y0 <= a.y1 ? b.z1 >= a.z0 && b.z0 <= a.z1 : 0) : 0;
}

static uint8_t x_intersects(AABB_t c, vec3_t v) {
    return vec3_is_null(v) ? 0 : v.y >= c.y0 && v.y <= c.y1 && v.z >= c.z0 && v.z <= c.z1;
}

static uint8_t y_intersects(AABB_t c, vec3_t v) {
    return vec3_is_null(v) ? 0 : v.x >= c.x0 && v.x <= c.x1 && v.z >= c.z0 && v.z <= c.z1;
}

static uint8_t z_intersects(AABB_t c, vec3_t v) {
    return vec3_is_null(v) ? 0 : v.x >= c.x0 && v.x <= c.x1 && v.y >= c.y0 && v.y <= c.y1;
}

hit_result_t AABB_clip(AABB_t c, vec3_t v1, vec3_t v0) {
    vec3_t f0 = vec3_clip_x(v0, v1, c.x0);
    vec3_t f1 = vec3_clip_x(v0, v1, c.x1);
    vec3_t f2 = vec3_clip_y(v0, v1, c.y0);
    vec3_t f3 = vec3_clip_y(v0, v1, c.y1);
    vec3_t f4 = vec3_clip_z(v0, v1, c.z0);
    vec3_t f5 = vec3_clip_z(v0, v1, c.z1);
    if(!x_intersects(c, f0)) f0 = vec3_null;
    if(!x_intersects(c, f1)) f1 = vec3_null;
    if(!y_intersects(c, f2)) f2 = vec3_null;
    if(!y_intersects(c, f3)) f3 = vec3_null;
    if(!z_intersects(c, f4)) f4 = vec3_null;
    if(!z_intersects(c, f5)) f5 = vec3_null;
    vec3_t f6 = vec3_null;
    if(!vec3_is_null(f0)) f6 = f0;
    if(!vec3_is_null(f1) && (vec3_is_null(f6) || vec3_distance_to_sqr(v1, f1) < vec3_distance_to_sqr(v1, f6))) f6 = f1;
    if(!vec3_is_null(f2) && (vec3_is_null(f6) || vec3_distance_to_sqr(v1, f2) < vec3_distance_to_sqr(v1, f6))) f6 = f2;
    if(!vec3_is_null(f3) && (vec3_is_null(f6) || vec3_distance_to_sqr(v1, f3) < vec3_distance_to_sqr(v1, f6))) f6 = f3;
    if(!vec3_is_null(f4) && (vec3_is_null(f6) || vec3_distance_to_sqr(v1, f4) < vec3_distance_to_sqr(v1, f6))) f6 = f4;
    if(!vec3_is_null(f5) && (vec3_is_null(f6) || vec3_distance_to_sqr(v1, f5) < vec3_distance_to_sqr(v1, f6))) f6 = f5;
    if(vec3_is_null(f6)) return (hit_result_t) { .null = 1 };
    int face = -1;
    if(f6.x == f0.x && f6.y == f0.y && f6.z == f0.z) face = 4;
    if(f6.x == f1.x && f6.y == f1.y && f6.z == f1.z) face = 5;
    if(f6.x == f2.x && f6.y == f2.y && f6.z == f2.z) face = 0;
    if(f6.x == f3.x && f6.y == f3.y && f6.z == f3.z) face = 1;
    if(f6.x == f4.x && f6.y == f4.y && f6.z == f4.z) face = 2;
    if(f6.x == f5.x && f6.y == f5.y && f6.z == f5.z) face = 3;
    return (hit_result_t){ .face = face, .location = f6 };
}