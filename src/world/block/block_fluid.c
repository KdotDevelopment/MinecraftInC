#include <level/block/block_still_liquid.h>
#include <level/block/block_liquid.h>
#include <level/block/blocks.h>
#include <level/block/block_sound.h>
#include <level/level.h>

block_t block_still_liquid_create(int block_type, int liquid_type) {
    block_t block = block_liquid_create(block_type, liquid_type);
    block_set_bounds(&block, 0.01, -0.1 + 0.01, 0.01, 1.01, 0.9 + 0.01, 1.0 + 0.01);
    block.liquid_moving_id = block_type - 1;
    block.liquid_still_id = block_type;
    block.has_physics = 0;
    block.should_tick = 0;

    block.update = block_still_liquid_update;
    block.on_neighbor_changed = block_still_liquid_on_neighbor_changed;

    block_list[block.id] = block;

    return block;
}

void block_still_liquid_update(block_t *block, struct level_s *level, int x, int y, int z, random_t *random) {
    //block_liquid_update(block, level, x, y, z, random);   
}

void block_still_liquid_on_neighbor_changed(block_t *block, struct level_s *level, int x, int y, int z, uint8_t block_id) {
    level_t *real_level = (level_t *)level;
    uint8_t update = 0;
    if(level_get_block(real_level, x - 1, y, z) == blocks.air.id) update = 1;
    if(level_get_block(real_level, x + 1, y, z) == blocks.air.id) update = 1;
    if(level_get_block(real_level, x, y - 1, z) == blocks.air.id) update = 1;
    if(level_get_block(real_level, x, y, z - 1) == blocks.air.id) update = 1;
    if(level_get_block(real_level, x, y, z + 1) == blocks.air.id) update = 1;
    if(block_id != blocks.air.id) {
        uint8_t liquid_type = block_list[block_id].liquid_type;
        if((block->liquid_type == LIQUID_WATER && liquid_type == LIQUID_LAVA) || (block->liquid_type == LIQUID_LAVA && liquid_type == LIQUID_WATER)) {
            level_set_block(real_level, x, y, z, blocks.cobblestone.id);
            return;
        }
    }
    if(update) {
        level_set_block_no_update(real_level, x, y, z, block->liquid_moving_id);
        level_add_to_next_tick(real_level, x, y, z, block->liquid_moving_id);
    }
    block_liquid_on_neighbor_changed(block, level, x, y, z, block_id);
}