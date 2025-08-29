#pragma once

#include <physics/AABB.h>
#include <renderer/block_renderer.h>
#include <world/world.h>

#include <stdint.h>

typedef struct chunk_renderer_s {
    world_t *world;
    int render_list;
    int x, y, z;
    int width, height, depth;
    int x_minus, y_minus, z_minus;
    int x_clip, y_clip, z_clip;
    int x_plus, y_plus, z_plus;
    uint8_t is_in_frustum;
    uint8_t skip_render[2];
    uint8_t needs_update;
    AABB_t bb;
    block_renderer_t block_renderer;
    uint8_t is_visible;
    uint8_t waiting_on_occlusion;
    int gl_occlusion_query;
    uint8_t is_lit;
} chunk_renderer_t;

void chunk_renderer_create(chunk_renderer_t *renderer, world_t *world, int x, int y, int z, int size, int render_list);
void chunk_renderer_set_position(chunk_renderer_t *renderer, int x, int y, int z);
void chunk_renderer_update(chunk_renderer_t *renderer);
float chunk_renderer_distance_to_entity_squared(chunk_renderer_t *renderer, entity_t *entity);
void chunk_renderer_dont_draw(chunk_renderer_t *renderer);
void chunk_renderer_stop_rendering(chunk_renderer_t *renderer);
int chunk_renderer_get_gl_call_list(chunk_renderer_t *renderer, int list);
void chunk_renderer_update_frustum(chunk_renderer_t *renderer, frustum_t *frustum);
void chunk_renderer_call_occlusion_query_list(chunk_renderer_t *renderer);
uint8_t chunk_renderer_skip_all_render_passes(chunk_renderer_t *renderer);
int chunk_renderer_compare(const void *a, const void *b);