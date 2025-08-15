#pragma once

#include <gui/progress_bar.h>

#include <util/random.h>

struct world_s;

typedef struct {
    int width;
    int height;
    int depth;
    random_t random;
    progress_bar_t *progress_bar;
    uint8_t *blocks;
    int water_world;
    int *flood_data; // 1024 * 1024
} world_gen_t;

world_gen_t world_gen_create();
void world_gen_generate(world_gen_t *world_gen, int width, int depth, struct world_s *proto_world);