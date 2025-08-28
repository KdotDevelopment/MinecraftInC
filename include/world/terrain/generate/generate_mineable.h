#pragma once

#include <util/random.h>
#include <world/world.h>

#include <stdint.h>

uint8_t generate_mineable_gen(world_t *world, random_t *random, int gen_x, int gen_y, int gen_z, uint8_t block_id);