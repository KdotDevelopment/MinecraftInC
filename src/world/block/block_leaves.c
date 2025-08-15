#include <world/block/block_leaves.h>
#include <world/block/blocks.h>
#include <world/block/block_sound.h>
#include <world/world.h>

block_t block_leaves_create() {
    block_t block = block_create(BLOCK_LEAVES, TEXTURE_LEAVES, block_sounds.grass, 0.4, 0.2, 1);
    block.is_opaque = 0;
    block.is_solid = 0;

    block.can_render_side = block_leaves_can_render_side;

    block_list[block.id] = block;

    return block;
}

uint8_t block_leaves_can_render_side(block_t *block, struct world_s *world, int x, int y, int z, int side) {
    world_t *real_world = (world_t *)world;
    return !world_is_solid_block(real_world, x, y, z);
}