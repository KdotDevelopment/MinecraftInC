#pragma once

#include <model/vertex.h>
#include <model/polygon.h>
#include <stdint.h>

#define PI 3.14159265358979323846

typedef struct {
    vertex_t vertices[8];
    polygon_t polygons[6];
    int x_texture_offset;
    int y_texture_offset;
    float x;
    float y;
    float z;
    float x_rot;
    float y_rot;
    float z_rot;
    uint8_t compiled;
    int list;
    uint8_t mirror;
    uint8_t visible;
    uint8_t never_render;
} cube_t;

cube_t cube_create(int x_texture_offset, int y_texture_offset);
void cube_add_box(cube_t *cube, float x, float y, float z, int w, int h, int d, float g);
void cube_set_pos(cube_t *cube, float x, float y, float z);
void cube_render(cube_t *cube, float scale);
void cube_compile(cube_t *cube, float scale);