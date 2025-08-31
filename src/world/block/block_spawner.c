#include <world/block/block_spawner.h>

#include <world/world.h>

block_t block_spawner_create(uint8_t block_id, uint8_t spawn_id) {
    block_t block = block_create(block_id, block_list[spawn_id].texture_id, &block_sounds.wood, 0.0, 0.0, &materials.water);

    block.spawner_liquid = spawn_id;
    block.should_tick = 1;

    block.on_added = block_spawner_on_added;
    block.update = block_spawner_update;

    return block;
}

void block_spawner_on_added(block_t *block, world_t *world, int x, int y, int z) {
    block_on_added(block, world, x, y, z);
    if(world_get_block(world, x - 1, y, z) == 0) {
        world_set_block(world, x - 1, y, z, block->spawner_liquid);
    }
    if(world_get_block(world, x + 1, y, z) == 0) {
        world_set_block(world, x + 1, y, z, block->spawner_liquid);
    }
    if(world_get_block(world, x, y, z - 1) == 0) {
        world_set_block(world, x, y, z - 1, block->spawner_liquid);
    }
    if(world_get_block(world, x, y, z + 1) == 0) {
        world_set_block(world, x, y, z + 1, block->spawner_liquid);
    }
}

void block_spawner_update(block_t *block, world_t *world, int x, int y, int z, random_t *random) {
    block_update(block, world, x, y, z, random);
    if(world_get_block(world, x - 1, y, z) == 0) {
        world_set_block(world, x - 1, y, z, block->spawner_liquid);
    }
    if(world_get_block(world, x + 1, y, z) == 0) {
        world_set_block(world, x + 1, y, z, block->spawner_liquid);
    }
    if(world_get_block(world, x, y, z - 1) == 0) {
        world_set_block(world, x, y, z - 1, block->spawner_liquid);
    }
    if(world_get_block(world, x, y, z + 1) == 0) {
        world_set_block(world, x, y, z + 1, block->spawner_liquid);
    }
}