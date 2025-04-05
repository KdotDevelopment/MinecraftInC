#include <util/sin_table.h>

static float sin_table[0x10000];

void sin_table_initialize() {
    for (int i = 0; i < sizeof(sin_table) / sizeof(float); i++) {
        sin_table[i] = sin(i * 2.0 * M_PI / (float)0xFFFF);
    }
}

float tsin(float x) {
    return sin_table[(int)(x * 10430.378f) & 0xFFFF];
}

float tcos(float x) {
    return sin_table[(int)(x * 10430.378f + 16384.0f) & 0xFFFF];
}