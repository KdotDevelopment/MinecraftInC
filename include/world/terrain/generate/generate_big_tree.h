#pragma once

#include <util/random.h>
#include <world/world.h>

typedef struct generate_big_tree_s {
    random_t *rand;
    world_t *world;
    vec3_t origin;
    int (*coords)[4];
    int coord_length;

    int height;
    int height_limit;
    int trunk_height;
    double trunk_height_scale;
    double branch_density;
    double branch_slope;
    double width_scale;
    double foliage_density;
    int trunk_width;
    int height_variance;
    int foliage_height;
} generate_big_tree_t;

uint8_t generate_big_tree_gen(world_t *world, random_t *random, int x, int y, int z);