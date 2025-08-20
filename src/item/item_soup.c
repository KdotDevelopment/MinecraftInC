#include <item/item_soup.h>

#include <item/items.h>
#include <item/item_food.h>

item_t item_soup_create(int16_t id, uint8_t texture_id, int16_t heal_amount) {
    item_t item = item_food_create(id, texture_id, heal_amount);

    item.on_right_click = item_soup_on_right_click;

    item_list[id + 256] = item;

    return item;
}

item_stack_t item_soup_on_right_click(item_stack_t *item_stack, world_t *world, player_t *player) {
    item_food_on_right_click(item_stack, world, player);

    item_stack_t empty_bowl = item_stack_create(ITEM_BOWL, 1, 0);

    return empty_bowl;
}