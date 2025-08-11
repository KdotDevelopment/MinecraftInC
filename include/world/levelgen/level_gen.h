#pragma once

#include <gui/progress_bar.h>

#include <util/random.h>

struct level_s;

typedef struct {
    int width;
    int height;
    int depth;
    random_t random;
    progress_bar_t *progress_bar;
    uint8_t *blocks;
    int water_level;
    int *flood_data; // 1024 * 1024
} level_gen_t;

level_gen_t level_gen_create();
void level_gen_generate(level_gen_t *level_gen, int width, int depth, struct level_s *proto_level);