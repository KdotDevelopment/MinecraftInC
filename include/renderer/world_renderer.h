#pragma once

#include <world/world.h>
#include <renderer/textures.h>
#include <renderer/chunk.h>
#include <player/player.h>

typedef struct world_renderer_s {
    world_t *world;
    textures_t *textures;
    int list_id;
    chunk_t **chunks; //array_list
    chunk_t **load_queue;
    chunk_t **chunk_cache;
    int chunk_cache_count;
    int x_chunks;
    int y_chunks;
    int z_chunks;
    int base_list_id;
    int *chunk_data_cache;
    int ticks;
    float last_load_x;
    float last_load_y;
    float last_load_z;
    float destroy_progress;
} world_renderer_t;

world_renderer_t world_renderer_create(struct minecraft_s *minecraft, world_t *world, textures_t *textures);
void world_renderer_refresh(world_renderer_t *renderer);
int world_renderer_sort_chunks(world_renderer_t *renderer, player_t *player, int pass);
void world_renderer_queue_chunks(world_renderer_t *renderer, int x0, int y0, int z0, int x1, int y1, int z1);
void world_renderer_destroy(world_renderer_t *renderer);