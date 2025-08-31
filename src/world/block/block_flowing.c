#include <world/block/block_flowing.h>

#include <world/block/blocks.h>
#include <world/block/block_fluid.h>
#include <world/block/block_sound.h>
#include <world/material/materials.h>
#include <world/world.h>

block_t block_flowing_create(uint8_t block_id, material_t *material) {
    block_t block = block_fluid_create(block_id, material);

    if(material == &materials.lava) {
        block.hardness = 0;
    }

    block.on_added = block_flowing_on_added;
    block.update = block_flowing_update;
    block.can_render_side = block_flowing_can_render_side;
    block.get_collision_aabb = block_flowing_get_collision_aabb;

    block_list[block.id] = block;

    return block;
}

void block_flowing_on_added(block_t *block, world_t *world, int x, int y, int z) {
    world_schedule_block_update(world, x, y, z, block->liquid_moving_id);
}

void block_flowing_update(block_t *block, world_t *world, int x, int y, int z, random_t *random) {
    return;
}

uint8_t block_flowing_can_render_side(block_t *block, world_t *world, int x, int y, int z, uint8_t side) {
    uint8_t block_id = world_get_block(world, x, y, z);
    if(block_id != block->liquid_moving_id && block_id != block->liquid_still_id) {
        if(side == 1) {
            uint8_t air_neighbor = world_get_block(world, x - 1, y, z) == 0 ||
                world_get_block(world, x + 1, y, z) == 0 ||
                world_get_block(world, x, y, z - 1) == 0 ||
                world_get_block(world, x, y, z + 1) == 0;
            if(air_neighbor) return 1;
        }
        return block_can_render_side(block, world, x, y, z, side);
    }else {
        return 0;
    }
}

AABB_t block_flowing_get_collision_aabb(block_t *block, int x, int y, int z) {
    return (AABB_t){ .null = 1 };
}