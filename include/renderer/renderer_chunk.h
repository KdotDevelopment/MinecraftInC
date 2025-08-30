#pragma once

#include <physics/AABB.h>
#include <renderer/renderer_block.h>
#include <world/world.h>

#include <stdint.h>

typedef struct renderer_chunk_s {
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
    renderer_block_t renderer_block;
    uint8_t is_visible;
    uint8_t waiting_on_occlusion;
    int gl_occlusion_query;
    uint8_t is_lit;
} renderer_chunk_t;

void renderer_chunk_create(renderer_chunk_t *renderer, world_t *world, int x, int y, int z, int size, int render_list);
void renderer_chunk_set_position(renderer_chunk_t *renderer, int x, int y, int z);
void renderer_chunk_update(renderer_chunk_t *renderer);
float renderer_chunk_distance_to_entity_squared(renderer_chunk_t *renderer, entity_t *entity);
void renderer_chunk_dont_draw(renderer_chunk_t *renderer);
void renderer_chunk_stop_rendering(renderer_chunk_t *renderer);
int renderer_chunk_get_gl_call_list(renderer_chunk_t *renderer, int list);
void renderer_chunk_update_frustum(renderer_chunk_t *renderer, frustum_t *frustum);
void renderer_chunk_call_occlusion_query_list(renderer_chunk_t *renderer);
uint8_t renderer_chunk_skip_all_render_passes(renderer_chunk_t *renderer);
int renderer_chunk_entity_compare(const void *a, const void *b);
int renderer_chunk_player_compare(const void *a, const void *b);