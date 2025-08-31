#include <item/item_hoe.h>

#include <entity/entity_item.h>
#include <item/items.h>
#include <util/random.h>
#include <world/block/blocks.h>

#include <stdlib.h>

item_t item_hoe_create(uint8_t id, uint8_t texture_id, uint8_t hoe_type) {
    item_t item = item_create(id, texture_id);

    item.max_stack_size = 1;
    item.durability = 32 << hoe_type;

    item.on_use = item_hoe_on_use;

    item_list[id + 256] = item;

    return item;
}

uint8_t item_hoe_on_use(item_stack_t *item_stack, world_t *world, int x, int y, int z, uint8_t side) {
    uint8_t block_id = world_get_block(world, x, y, z);
    if(/*(material.is_solid || block_id != BLOCK_GRASS) && block_id != BLOCK_DIRT*/ block_id != BLOCK_GRASS && block_id != BLOCK_DIRT) {
        return 0;
    }
    // Make sound
    item_stack_damage(item_stack, 1);
    
    if(random_next_int_range(&world->random, 0, 8) == 0 && block_id == BLOCK_GRASS) {
        // There was a for loop here (that iterates once) presumably to change the amount of seeds to drop... fun fact
        float offset_x = random_next_uniform(&world->random) * 0.7 + 0.15;
        float offset_z = random_next_uniform(&world->random) * 0.7 + 0.15;
        entity_t *item = malloc(sizeof(entity_t));
        item_stack_t item_stack = item_stack_create(ITEM_SEEDS, 1, 0);
        entity_item_create(item, world, x + offset_x, y + 1.2, z + offset_z, item_stack);
        item->delay_before_pickup = 10;
        world_spawn_entity(world, item);
    }

    return 1;
}