#include <item/item.h>

// The first 256 are filled by blocks, everything 256+ is regular items
item_t item_list[1024] = { 0 };

item_t item_create(int16_t id, uint8_t texture_id) {
    item_t item = { 0 };

    item.item_id = id + 256;
    item.texture_id = texture_id;

    item.on_use = item_on_use;
    item.on_right_click = item_on_right_click;
    item.get_strength_against_block = item_get_strength_against_block;
    item.hit_entity = item_hit_entity;
    item.on_block_destroy = item_on_block_destroy;
    item.get_damage_against_entity = item_get_damage_against_entity;

    item_list[id + 256] = item;

    return item;
}

uint8_t item_on_use(item_t *item, item_stack_t *item_stack, world_t *world, int x, int y, int z, uint8_t side) {
    return 0;
}

item_stack_t item_on_right_click(item_stack_t *item_stack, world_t *world, player_t *player) {
    return *item_stack;
}

float item_get_strength_against_block(item_t *item, block_t *block) {
    return 1;
}

void item_hit_entity(item_t *item, item_stack_t *item_stack) {
    return;
}

void item_on_block_destroy(item_t *item, item_stack_t *item_stack) {
    return;
}

int item_get_damage_against_entity(item_t *item) {
    return 1;
}