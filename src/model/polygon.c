#include <model/polygon.h>
#include <string.h>

polygon_t polygon_create(vertex_t vertices[4], int u1, int v1, int u2, int v2) {
    polygon_t polygon;
    memset(&polygon, 0, sizeof(polygon_t));
    float a = 0.0015625;
    float b = 0.003125;

    polygon.vertices[0] = vertex_remap(&vertices[0], u2 / 64.0 - a, v1 / 32.0 + b);
    polygon.vertices[1] = vertex_remap(&vertices[1], u1 / 64.0 + a, v1 / 32.0 + b);
    polygon.vertices[2] = vertex_remap(&vertices[2], u1 / 64.0 + a, v2 / 32.0 - b);
    polygon.vertices[3] = vertex_remap(&vertices[3], u2 / 64.0 - a, v2 / 32.0 - b);

    return polygon;
}

polygon_t polygon_create_float(vertex_t vertices[4], float u1, float v1, float u2, float v2) {
    polygon_t polygon;
    memset(&polygon, 0, sizeof(polygon_t));

    polygon.vertices[0] = vertex_remap(&vertices[0], u2, v1);
    polygon.vertices[1] = vertex_remap(&vertices[1], u1, v1);
    polygon.vertices[2] = vertex_remap(&vertices[2], u1, v2);
    polygon.vertices[3] = vertex_remap(&vertices[3], u2, v2);

    return polygon;
}

void polygon_flip(polygon_t *polygon) {
    vertex_t vertices[4];

    for (uint8_t j = 0; j < 4; j++) {
        vertices[j] = polygon->vertices[3 - j];
    }

    polygon->vertices[0].u = vertices[1].u;
    polygon->vertices[1].u = vertices[0].u;
    polygon->vertices[2].u = vertices[3].u;
    polygon->vertices[3].u = vertices[2].u;
}