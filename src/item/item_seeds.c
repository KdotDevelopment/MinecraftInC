#include <item/item_seeds.h>

#include <world/block/blocks.h>

item_t item_seeds_create(uint8_t id, uint8_t texture_id, uint8_t block_type) {
    item_t item = item_create(id, texture_id);

    item.block_id = block_type;
    
    item.on_use = item_seeds_on_use;

    item_list[id + 256] = item;

    return item;
}

uint8_t item_seeds_on_use(item_t *item, item_stack_t *item_stack, world_t *world, int x, int y, int z, uint8_t side) {
    if(side != 1) return 0;

    uint8_t block_id = world_get_block(world, x, y, z);
    if(block_id == BLOCK_FARMLAND) {
        world_set_block_with_update(world, x, y + 1, z, item_list[item_stack->item_id].block_id);
        item_stack->stack_size--;
        return 1;
    }
    return 0;
}