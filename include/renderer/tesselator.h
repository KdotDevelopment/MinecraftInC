#pragma once

#include <stdint.h>

#define VBO_COUNT 10

typedef struct {
    uint8_t byte_buffer[8388608];
    int raw_buffer[2097152];
    uint32_t vertex_buffer[VBO_COUNT];
    uint32_t color;
    int vertex_count; // = 0
    double u, v;
    int raw_buffer_index; // = 0
    int added_vertices; // = 0
    uint8_t draw_mode;
    double x_offset, y_offset, z_offset;
    int vbo_index; // = 0
    uint8_t convert_quads_to_triangles; // = 0
    uint8_t try_vbo; // = 0
    uint8_t is_drawing; // = 0
    uint8_t use_vbo; // = 0
    uint8_t has_color; // = 0
    uint8_t has_texture; // = 0
    uint8_t no_color; // = 0
} tesselator_t;

extern tesselator_t g_tesselator;

void tesselator_create(tesselator_t *tesselator);
void tesselator_end();
void tesselator_clear();
void tesselator_begin_quads();
void tesselator_begin(int mode);
void tesselator_color_float(float r, float g, float b, float a);
void tesselator_color_opaque_int(uint32_t color);
void tesselator_color_opaque(uint8_t r, uint8_t g, uint8_t b);
void tesselator_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void tesselator_vertex_uv(double x, double y, double z, double u, double v);
void tesselator_vertex(double x, double y, double z);
void tesselator_color_int(uint32_t color);
void tesselator_normal(float x, float y, float z);
void tesselator_disable_color();
void tesselator_set_translation(double x, double y, double z);