#include <item/item.h>

// The first 256 are filled by blocks, everything 256+ is regular items
item_t item_list[1024] = { 0 };

item_t item_create(int16_t id, uint8_t texture_id) {
    item_t item = { 0 };

    item.item_id = id + 256;
    item.texture_id = texture_id;

    item_list[id + 256] = item;

    item.on_use = item_on_use;
    item.on_right_click = item_on_right_click;

    return item;
}

uint8_t item_on_use(item_stack_t *item_stack, world_t *world, int x, int y, int z, uint8_t side) {
    return 0;
}

item_stack_t item_on_right_click(item_stack_t *item_stack, world_t *world, player_t *player) {
    return *item_stack;
}