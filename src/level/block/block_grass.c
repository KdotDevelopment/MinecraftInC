#include <level/block/block_grass.h>
#include <level/block/blocks.h>
#include <level/block/block_sound.h>
#include <level/level.h>

block_t block_grass_create() {
    block_t block = block_create(BLOCK_GRASS, TEXTURE_GRASS, block_sounds.grass, 1, 0.6, 1);
    block.has_physics = 1;
    block.should_tick = 1;

    block.get_texture_id = block_grass_get_texture_id;
    block.update = block_grass_update;

    block_list[block.id] = block;

    return block;
}

int block_grass_get_texture_id(block_t *block, int side) {
    return side == 1 ? TEXTURE_GRASS : (side == 0 ? TEXTURE_DIRT : TEXTURE_GRASS_SIDE);
}

void block_grass_update(block_t *block, struct level_s *level, int x, int y, int z, random_t *random) {
    level_t *real_level = (level_t *)level;
    if(random_next_int_range(random, 0, 3) == 0) {
        if(!level_is_lit(real_level, x, y, z)) {
            level_set_block(real_level, x, y, z, blocks.dirt.id);
        }else {
            for(int i = 0; i < 4; i++) {
                int xx = x + random_next_int_range(random, 0, 2) - 1;
                int yy = y + random_next_int_range(random, 0, 4) - 3;
                int zz = z + random_next_int_range(random, 0, 2) - 1;
                if(level_get_block(real_level, xx, yy, zz) == blocks.dirt.id && level_is_lit(real_level, xx, yy, zz)) {
                    level_set_block(real_level, xx, yy, zz, blocks.grass.id);
                }
            }
        }
    }
}