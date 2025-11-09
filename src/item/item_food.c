#include <item/item_food.h>

item_t item_food_create(uint8_t id, uint8_t texture_id, uint8_t heal_amount) {
    item_t item = item_create(id, texture_id);

    item.heal_amount = heal_amount;
    item.max_stack_size = 1;

    item.on_right_click = item_food_on_right_click;

    item_list[id + 256] = item;

    return item;
}

item_stack_t item_food_on_right_click(item_stack_t *item_stack, world_t *world, player_t *player) {
    item_stack_t item = *item_stack;
    item.stack_size--;
    mob_heal(player->mob->entity, item_list[item_stack->item_id].heal_amount);
    return item;
}