#include <world/block/block_glass.h>
#include <world/block/blocks.h>
#include <world/block/block_sound.h>
#include <world/world.h>

block_t block_glass_create() {
    block_t block = block_create(BLOCK_GLASS, TEXTURE_GLASS, block_sounds.metal, 1, 0.3, 1);
    block.can_render_side = block_glass_can_render_side;
    block.is_opaque = 0;
    block.is_solid = 0;

    block_list[block.id] = block;

    return block;
}

uint8_t block_glass_can_render_side(block_t *block, struct world_s *world, int x, int y, int z, int side) {
    world_t *real_world = (world_t *)world;
    uint8_t block_id = world_get_block(real_world, x, y, z);
    return block_id == block->id ? 0 : !world_is_solid_block(real_world, x, y, z);
}