#pragma once

#include <stdint.h>

typedef struct {
    float buffer[524288];
    int vertices; // = 0
    float u, v;
    uint32_t color;
    int len; // = 3
    int p;
    uint8_t has_color; // = 0
    uint8_t has_texture; // = 0
    uint8_t no_color; // = 0
} tesselator_t;

extern tesselator_t g_tesselator;

void tesselator_create(tesselator_t *tesselator);
void tesselator_end();
void tesselator_clear();
void tesselator_begin();
void tesselator_color_float(float r, float g, float b, float a);
void tesselator_color_opaque_int(uint32_t color);
void tesselator_color_opaque(uint8_t r, uint8_t g, uint8_t b);
void tesselator_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void tesselator_vertex_uv(float x, float y, float z, float u, float v);
void tesselator_vertex(float x, float y, float z);
void tesselator_color_int(uint32_t color);
void tesselator_normal(float x, float y, float z);