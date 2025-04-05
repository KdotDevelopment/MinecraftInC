#include <level/block/block_sand.h>
#include <level/block/blocks.h>
#include <level/level.h>

block_t block_sand_create(uint8_t block_id, int texture_id) {
    block_t block = block_create(block_id, texture_id, block_sounds.gravel, 1, block_id == BLOCK_SAND ? 0.5 : 0.6, 1);
    block.on_neighbor_changed = block_sand_on_neighbor_placed;
    block.on_placed = block_sand_on_placed;

    block_list[block.id] = block;

    return block;
}

void fall(block_t *block, struct level_s *level, int x, int y, int z) {
    level_t *real_level = (level_t *)level;
    int vx = x;
    int vy = y;
    int vz = z;
    for(;;) {
        uint8_t block_id = level_get_block(real_level, vx, vy - 1, vz);
        uint8_t liquid_type = block_id == blocks.air.id ? LIQUID_NONE : block_list[block_id].liquid_type;
        if(!(block_id == blocks.air.id ? 1 : (liquid_type == LIQUID_WATER ? 1 : liquid_type == LIQUID_LAVA)) || vy <= 0) {
            if(y != vy) {
                block_id = level_get_block(real_level, vx, vy, vz);
                if(block_id != blocks.air.id && block_list[block_id].liquid_type != LIQUID_NONE) {
                    level_set_block_no_update(real_level, vx, vy, vz, blocks.air.id);
                }
                level_swap(real_level, x, y, z, vx, vy, vz);
            }
            return;
        }
        vy--;
    }
}

void block_sand_on_neighbor_placed(block_t *block, struct level_s *level, int x, int y, int z, uint8_t block_id) {
    fall(block, level, x, y, z);
}

void block_sand_on_placed(block_t *block, struct level_s *level, int x, int y, int z) {
    fall(block, level, x, y, z);
}