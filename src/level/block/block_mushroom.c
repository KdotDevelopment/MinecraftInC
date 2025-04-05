#include <level/block/block_mushroom.h>
#include <level/block/block_flower.h>
#include <level/block/blocks.h>
#include <level/block/block_sound.h>
#include <level/level.h>

block_t block_mushroom_create(uint8_t id, int texture_id) {
    block_t block = block_flower_create(id, texture_id);
    block.update = block_mushroom_update;
    block_set_bounds(&block, 0.3, 0, 0.3, 0.7, 0.4, 0.7);

    block_list[block.id] = block;

    return block;
}

void block_mushroom_update(block_t *block, struct level_s *level, int x, int y, int z, random_t *random) {
    level_t *real_level = (level_t *)level;
    uint8_t block_id = level_get_block(real_level, x, y - 1, z);
    if(level_is_lit(real_level, x, y, z) || (block_id != blocks.stone.id && block_id != blocks.gravel.id && block_id != blocks.cobblestone.id)) {
        level_set_block(real_level, x, y, z, blocks.air.id);
    }
}