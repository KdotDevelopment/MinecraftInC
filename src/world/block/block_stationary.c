#include <world/block/block_stationary.h>

#include <world/block/block_fire.h>
#include <world/block/block_fluid.h>
#include <world/block/blocks.h>
#include <world/world.h>

block_t block_stationary_create(uint8_t block_id, material_t *material) {
    block_t block = block_fluid_create(block_id, material);

    block.liquid_moving_id = block.id - 1;
    block.liquid_still_id = block.id;
    block.should_tick = 0;

    block.update = block_stationary_update;
    block.on_neighbor_changed = block_stationary_on_neighbor_changed;

    return block;
}

void block_stationary_update(block_t *block, world_t *world, int x, int y, int z, random_t *random) {
    return;
}

void block_stationary_on_neighbor_changed(block_t *block, world_t *world, int x, int y, int z, uint8_t block_id) {
    uint8_t can_flow = 0;
    if(block_fluid_can_flow(block, world, x, y - 1, z)) {
        can_flow = 1;
    }

    if(!can_flow && block_fluid_can_flow(block, world, x - 1, y, z)) {
        can_flow = 1;
    }

    if(!can_flow && block_fluid_can_flow(block, world, x + 1, y, z)) {
        can_flow = 1;
    }

    if(!can_flow && block_fluid_can_flow(block, world, x, y, z - 1)) {
        can_flow = 1;
    }

    if(!can_flow && block_fluid_can_flow(block, world, x, y, z + 1)) {
        can_flow = 1;
    }

    if(block_id != 0) {
        material_t *material = block_list[block_id].material;
        if((block->material == &materials.water && material == &materials.lava) || (material == &materials.water && block->material == &materials.lava)) {
            world_set_block_with_update(world, x, y, z, blocks.stone.id);
            return;
        }
    }

    if(block_fire_can_neighbor_spread_fire(block_id)) {
        can_flow = 1;
    }

    if(can_flow) {
        world_set_block_no_update(world, x, y, z, block->liquid_moving_id);
        world_schedule_block_update(world, x, y, z, block->liquid_moving_id);
    }
}