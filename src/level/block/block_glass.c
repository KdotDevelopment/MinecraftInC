#include <level/block/block_glass.h>
#include <level/block/blocks.h>
#include <level/block/block_sound.h>
#include <level/level.h>

block_t block_glass_create() {
    block_t block = block_create(BLOCK_GLASS, TEXTURE_GLASS, block_sounds.metal, 1, 0.3, 1);
    block.can_render_side = block_glass_can_render_side;
    block.is_opaque = 0;
    block.is_solid = 0;

    block_list[block.id] = block;

    return block;
}

uint8_t block_glass_can_render_side(block_t *block, struct level_s *level, int x, int y, int z, int side) {
    level_t *real_level = (level_t *)level;
    uint8_t block_id = level_get_block(real_level, x, y, z);
    return block_id == block->id ? 0 : !level_is_solid_block(real_level, x, y, z);
}