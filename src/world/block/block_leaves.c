#include <level/block/block_leaves.h>
#include <level/block/blocks.h>
#include <level/block/block_sound.h>
#include <level/level.h>

block_t block_leaves_create() {
    block_t block = block_create(BLOCK_LEAVES, TEXTURE_LEAVES, block_sounds.grass, 0.4, 0.2, 1);
    block.is_opaque = 0;
    block.is_solid = 0;

    block.can_render_side = block_leaves_can_render_side;

    block_list[block.id] = block;

    return block;
}

uint8_t block_leaves_can_render_side(block_t *block, struct level_s *level, int x, int y, int z, int side) {
    level_t *real_level = (level_t *)level;
    return !level_is_solid_block(real_level, x, y, z);
}