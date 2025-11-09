#include <world/block/block_leaves.h>

#include <world/block/blocks.h>
#include <world/block/block_sound.h>
#include <world/world.h>
#include <world/material/materials.h>

block_t block_leaves_create() {
    block_t block = block_create(BLOCK_LEAVES, TEXTURE_LEAVES, &block_sounds.grass, 0.2, 0, &materials.leaves);
    block.is_opaque = 0;
    block.light_opacity = 0;
    block.is_solid = 0;

    block.can_render_side = block_leaves_can_render_side;
    block.get_drop_count = block_leaves_get_drop_count;
    block.get_item_dropped = block_leaves_get_item_dropped;

    block_list[block.id] = block;

    return block;
}

uint8_t block_leaves_can_render_side(block_t *block, struct world_s *world, int x, int y, int z, uint8_t side) {
    world_t *real_world = (world_t *)world;
    return !world_is_solid(real_world, x, y, z);
}

uint8_t block_leaves_get_drop_count(block_t *block, random_t *random) {
    return random_next_int_range(random, 0, 9) == 0 ? 1 : 0;
}

int16_t block_leaves_get_item_dropped(block_t *block, uint8_t metadata, random_t *random) {
    return blocks.sapling.id;
}