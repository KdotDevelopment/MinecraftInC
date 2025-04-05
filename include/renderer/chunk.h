#pragma once

#include <level/level.h>
#include <player/player.h>
#include <renderer/frustum.h>

#include <stdint.h>

extern int chunk_updates;

typedef struct {
    level_t *level;
    int base_list_id;
    int x, y, z;
    int width, height, depth;
    uint8_t visible;
    uint8_t dirty[2];
    uint8_t loaded;
} chunk_t;

chunk_t chunk_create(level_t *level, int x, int y, int z, int chunk_size, int base_list_id);
void chunk_update(chunk_t *chunk);
float chunk_distance_squared(chunk_t *chunk, player_t *player);
void chunk_set_all_dirty(chunk_t *chunk);
void chunk_dispose(chunk_t *chunk);
int chunk_append_lists(chunk_t *chunk, int data_cache[], int count, int pass);
void chunk_clip(chunk_t *chunk, frustum_t frustum);

int chunk_list_comparator(const void *a, const void *b);
int chunk_visible_distance_comparator(const void *a, const void *b);