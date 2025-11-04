#include <world/block/block_sand.h>
#include <world/block/blocks.h>
#include <world/world.h>
#include <world/material/materials.h>

block_t block_sand_create(uint8_t block_id, int texture_id, float hardness, float resistance) {
    block_t block = block_create(block_id, texture_id, &block_sounds.gravel, hardness, resistance, &materials.sand);

    block.on_neighbor_changed = block_sand_on_neighbor_placed;
    block.on_added = block_sand_on_added;

    block_list[block.id] = block;

    return block;
}

void fall(block_t *block, struct world_s *world, int x, int y, int z) {
    world_t *real_world = (world_t *)world;
    int vx = x;
    int vy = y;
    int vz = z;

    for(;;) {
        if(vy - 1 < 0) {
            world_set_block_no_update(real_world, vx, vy, vz, blocks.air.id);
            return;
        }

        uint8_t block_id = world_get_block(real_world, vx, vy - 1, vz);
        uint8_t liquid_type =
            (block_id == blocks.air.id || block_id == blocks.fire.id ||
             block_list[block_id].material == &materials.water ||
             block_list[block_id].material == &materials.lava);

        if(!liquid_type || vy < 0) {
            if(vy < 0) {
                world_set_block_no_update(real_world, vx, vy, vz, blocks.air.id);
            }

            if(y != vy) {
                block_id = world_get_block(real_world, vx, vy, vz);
                if(block_id != blocks.air.id &&
                    block_list[block_id].material != &materials.air) {
                        world_set_block_no_update(real_world, vx, vy, vz, blocks.air.id);
                    }
                world_swap(real_world, x, y, z, vx, vy, vz);
            }
            return;
        }

        vy--;

        if(world_get_block(real_world, vx, vy, vz) == blocks.fire.id) {
            world_set_block_no_update(real_world, vx, vy, vz, blocks.air.id);
        }
    }
}

void block_sand_on_neighbor_placed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t block_id) {
    fall(block, world, x, y, z);
}

void block_sand_on_added(block_t *block, struct world_s *world, int x, int y, int z) {
    fall(block, world, x, y, z);
}