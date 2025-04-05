#include <model/vertex.h>

vertex_t vertex_remap(vertex_t *a, float u, float v) {
    return (vertex_t){ a->pos, u, v };
}