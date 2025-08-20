#include <item/item_block.h>

#include <world/block/blocks.h>

item_t item_block_create(int16_t block_id) {
    item_t item = item_create(block_id, block_list[block_id + 256].get_texture_id(&block_list[block_id + 256], 2));
    
    item.block_id = block_id + 256;

    item.on_use = item_block_on_use;
    
    // item_list is done in blocks.c

    return item;
}

uint8_t item_block_on_use(item_stack_t *item_stack, world_t *world, int x, int y, int z, uint8_t side) {
    if(side == 0) y--;
    if(side == 1) y++;
    if(side == 2) z--;
    if(side == 3) z++;
    if(side == 4) x--;
    if(side == 5) x++;

    if(item_stack->stack_size == 0) return 0;

    uint8_t block_id = world_get_block(world, x, y, z);
    block_t *block = &block_list[block_id];
    AABB_t aabb = block->get_collision_aabb(&block, x, y, z);

    if((block_id > 0 && block == NULL) || block_id == BLOCK_WATER || block_id == BLOCK_STILL_WATER || block_id == BLOCK_LAVA || block_id == BLOCK_STILL_LAVA || block_id == BLOCK_FIRE) {
        if(aabb.null /*|| world_check_aabb_clear(world, aabb) || block->can_place_at(block, world, x, y, z)*/ && world_set_block(world, x, y, z, item_stack->item_id)) {
            block->on_placed(block, world, x, y, z);
            // TODO: Play sound at coords
            item_stack->stack_size--;
        }
    }

    return 0;
}