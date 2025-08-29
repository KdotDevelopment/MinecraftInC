#pragma once

#include <player/player.h>
#include <renderer/block_renderer.h>
#include <renderer/chunk_renderer.h>
#include <renderer/textures.h>
#include <world/world.h>

struct minecraft_s;

typedef struct world_renderer_s {
    world_t *world;
    struct minecraft_s *minecraft;
    textures_t *textures;
    int list_id;
    chunk_renderer_t **chunk_renderers_sorted;
    chunk_renderer_t **chunk_renderers;
    uint64_t *chunk_renderers_to_update; // array_list
    int chunk_renderer_count;
    int chunk_cache_count;
    int x_chunks;
    int y_chunks;
    int z_chunks;
    int render_list_base;
    int *render_lists; // array_list
    int occlusion_query_base[262144];
    int *chunk_data_cache;
    int ticks;
    float last_load_x;
    float last_load_y;
    float last_load_z;
    float destroy_progress;
    int cloud_offset_x;
    uint8_t occlusion_enabled;
    int star_render_list;
    int sky_render_list;
    block_renderer_t block_renderer;
    int render_distance;
    int x0, y0, z0;
    int x1, y1, z1;
    int entities_total;
    int entities_rendered;
    int entities_hidden;
} world_renderer_t;

world_renderer_t world_renderer_create(struct minecraft_s *minecraft, world_t *world, textures_t *textures);
void world_renderer_refresh(world_renderer_t *renderer);
void world_renderer_queue_chunks(world_renderer_t *renderer, int x0, int y0, int z0, int x1, int y1, int z1);
void world_renderer_update_block(world_renderer_t *renderer, int x, int y, int z);
void world_renderer_update_blocks(world_renderer_t *renderer, int x0, int y0, int z0, int x1, int y1, int z1);
void world_renderer_update_frustum(world_renderer_t *renderer, frustum_t *frustum);
void world_renderer_play_sound(world_renderer_t *renderer, uint8_t sound, double x, double y, double z, float volume, float pitch);
void world_renderer_spawn_particle(world_renderer_t *renderer, uint8_t particle_type, double x, double y, double z, double x_vel, double y_vel, double z_vel);
void world_renderer_destroy(world_renderer_t *renderer);