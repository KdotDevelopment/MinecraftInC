#include <item/item_painting.h>

#include <entity/entity_painting.h>

#include <stdlib.h>

item_t item_painting_create(uint8_t id, uint8_t texture_id) {
    item_t item = item_create(id, texture_id);

    // Unknown purpose, is in original code
    item.durability = 64;

    item.on_use = item_painting_on_use;

    item_list[id + 256] = item;

    return item;
}

uint8_t item_painting_on_use(item_stack_t *item_stack, world_t *world, int x, int y, int z, uint8_t side) {
    if(side == 0) return 0;
    if(side == 1) return 0;
    int8_t new_side = 0;
    if(side == 4) new_side = 1;
    if(side == 3) new_side = 2;
    if(side == 5) new_side = 3;

    entity_t *painting = malloc(sizeof(entity_t));
    entity_painting_create(painting, world, x, y, z, new_side);
       
    if(entity_painting_on_valid_surface(painting)) {
        world_spawn_entity(world, painting);
        item_stack->stack_size--;
    }

    return 1;
}