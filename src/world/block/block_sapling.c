#include <world/block/block_sapling.h>

#include <world/block/block_flower.h>
#include <world/block/blocks.h>
#include <world/block/block_sound.h>
#include <world/terrain/generate/generate_big_tree.h>
#include <world/world.h>

block_t block_sapling_create() {
    block_t block = block_flower_create(BLOCK_SAPLING, TEXTURE_SAPLING);
    block.update = block_sapling_update;
    block_set_bounds(&block, 0.1, 0, 0.1, 0.9, 0.8, 0.9);

    block_list[block.id] = block;

    return block;
}

void block_sapling_update(block_t *block, world_t *world, int x, int y, int z, random_t *random) {
    block_flower_update(block, world, x, y, z, random);
    uint8_t block_id = world_get_block(world, x, y - 1, z);
    if(world_get_block_light_value(world, x, y, z) >= 9 && random_next_int_range(random, 0, 4) == 0) {
        uint8_t metadata = world_get_block_metadata(world, x, y, z);
        if(metadata < 15) {
            world_set_block_metadata(world, x, y, z, metadata + 1);
            return;
        }
        world_set_block_no_update(world, x, y, z, blocks.air.id);
        if(!generate_big_tree_gen(world, random, x, y, z)) {
            world_set_block_no_update(world, x, y, z, block->id);
        }
    }
    return;
}