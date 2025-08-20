#include <item/item_bow.h>

item_t item_bow_create(uint8_t id, uint8_t texture_id) {
    item_t item = item_create(id, texture_id);

    item.max_stack_size = 1;

    item.on_right_click = item_bow_on_right_click;

    item_list[id + 256] = item;

    return item;
}

item_stack_t item_bow_on_right_click(item_stack_t *item_stack, world_t *world, player_t *player) {
    // consume item inventory
    // spawn item in world
    return *item_stack;
}