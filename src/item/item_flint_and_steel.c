#include <item/item_flint_and_steel.h>

#include <world/block/blocks.h>

item_t item_flint_and_steel_create(uint8_t id, uint8_t texture_id) {
    item_t item = item_create(id, texture_id);

    item.max_stack_size = 1;
    item.durability = 64;

    item.on_use = item_flint_and_steel_on_use;

    item_list[id + 256] = item;

    return item;
}

uint8_t item_flint_and_steel_on_use(item_stack_t *item_stack, world_t *world, int x, int y, int z, uint8_t side) {
    if(side == 0) y--;
    if(side == 1) y++;
    if(side == 2) z--;
    if(side == 3) z++;
    if(side == 4) x--;
    if(side == 5) x++;

    uint8_t block_id = world_get_block(world, x, y, z);
    if(block_id == 0) {
        // sound fire.ignite
        world_set_block(world, x, y, z, BLOCK_FIRE);
    }
    item_stack_damage(item_stack, 1);

    return 1;   
}