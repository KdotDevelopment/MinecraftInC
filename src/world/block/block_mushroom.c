#include <world/block/block_mushroom.h>
#include <world/block/block_flower.h>
#include <world/block/blocks.h>
#include <world/block/block_sound.h>
#include <world/world.h>

block_t block_mushroom_create(uint8_t id, int texture_id) {
    block_t block = block_flower_create(id, texture_id);
    block.update = block_mushroom_update;
    block_set_bounds(&block, 0.3, 0, 0.3, 0.7, 0.4, 0.7);

    block_list[block.id] = block;

    return block;
}

void block_mushroom_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random) {
    world_t *real_world = (world_t *)world;
    uint8_t block_id = world_get_block(real_world, x, y - 1, z);
    if(world_is_lit(real_world, x, y, z) || (block_id != blocks.stone.id && block_id != blocks.gravel.id && block_id != blocks.cobblestone.id)) {
        world_set_block(real_world, x, y, z, blocks.air.id);
    }
}