#include <world/block/block_mushroom.h>

#include <world/block/block_flower.h>
#include <world/block/blocks.h>
#include <world/block/block_sound.h>
#include <world/world.h>

block_t block_mushroom_create(uint8_t id, int texture_id) {
    block_t block = block_flower_create(id, texture_id);

    block_set_bounds(&block, 0.3, 0, 0.3, 0.7, 0.4, 0.7);

    block.can_grow_on = block_mushroom_can_grow_on;
    block.can_stay = block_mushroom_can_stay;

    block_list[block.id] = block;

    return block;
}

uint8_t block_mushroom_can_grow_on(block_t *block, uint8_t block_id) {
    return block_list[block_id].is_opaque;
}

uint8_t block_mushroom_can_stay(block_t *block, world_t *world, int x, int y, int z) {
    if(world_get_block_light_value(world, x, y, z) <= 13) {
        uint8_t block_id = world_get_block(world, x, y - 1, z);
        if(block_list[block_id].is_opaque) return 1;
    }
    return 0;
}