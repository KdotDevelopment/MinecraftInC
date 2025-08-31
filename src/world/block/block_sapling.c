#include <world/block/block_sapling.h>
#include <world/block/block_flower.h>
#include <world/block/blocks.h>
#include <world/block/block_sound.h>
#include <world/world.h>

block_t block_sapling_create() {
    block_t block = block_flower_create(BLOCK_SAPLING, TEXTURE_SAPLING);
    block.update = block_sapling_update;
    block_set_bounds(&block, 0.1, 0, 0.1, 0.9, 0.8, 0.9);

    block_list[block.id] = block;

    return block;
}

void block_sapling_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random) {
    world_t *real_world = (world_t *)world;
    uint8_t block_id = world_get_block(real_world, x, y - 1, z);
    if(world_is_lit(real_world, x, y, z) && (block_id == blocks.dirt.id || block_id == blocks.grass.id)) {
        if(random_next_int_range(random, 0, 4) == 0) {
            world_set_block_no_update(real_world, x, y, z, blocks.air.id);
            if(!world_maybe_grow_tree(real_world, x, y, z)) {
                world_set_block_no_update(real_world, x, y, z, block->id);
            }
        }
    }else {
        world_set_block_with_update(real_world, x, y, z, blocks.air.id);
    }
    return;
}