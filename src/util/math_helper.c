#include <util/math_helper.h>

static float math_helper[0x10000];

void math_helper_initialize() {
    for (int i = 0; i < sizeof(math_helper) / sizeof(float); i++) {
        math_helper[i] = sin(i * 2.0 * M_PI / (float)0xFFFF);
    }
}

float tsin(float x) {
    return math_helper[(int)(x * 10430.378f) & 0xFFFF];
}

float tcos(float x) {
    return math_helper[(int)(x * 10430.378f + 16384.0f) & 0xFFFF];
}

int floor_float(float x) {
    int xx = (int)x;
    return x < (float)xx ? xx - 1 : xx;
}

int floor_double(double x) {
    int xx = (int)x;
    return x < (double)xx ? xx - 1 : xx;
}