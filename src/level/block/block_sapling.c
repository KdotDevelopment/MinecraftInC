#include <level/block/block_sapling.h>
#include <level/block/block_flower.h>
#include <level/block/blocks.h>
#include <level/block/block_sound.h>
#include <level/level.h>

block_t block_sapling_create() {
    block_t block = block_flower_create(BLOCK_SAPLING, TEXTURE_SAPLING);
    block.update = block_sapling_update;
    block_set_bounds(&block, 0.1, 0, 0.1, 0.9, 0.8, 0.9);

    block_list[block.id] = block;

    return block;
}


void block_sapling_update(block_t *block, struct level_s *level, int x, int y, int z, random_t *random) {
    level_t *real_level = (level_t *)level;
    uint8_t block_id = level_get_block(real_level, x, y - 1, z);
    if(level_is_lit(real_level, x, y, z) && (block_id == blocks.dirt.id || block_id == blocks.grass.id)) {
        if(random_next_int_range(random, 0, 4) == 0) {
            level_set_block_no_update(real_level, x, y, z, blocks.air.id);
            if(!level_maybe_grow_tree(real_level, x, y, z)) {
                level_set_block_no_update(real_level, x, y, z, block->id);
            }
        }
    }else {
        level_set_block(real_level, x, y, z, blocks.air.id);
    }
    return;
}