#include <item/item_armor.h>

static int damage_reduce_amounts[] = { 3, 8, 6, 3 };
static int max_damages[] = { 11, 16, 15, 13 };

item_t item_armor_create(uint8_t id, uint8_t texture_id, uint8_t armor_type, uint8_t piece) {
    item_t item = item_create(id, texture_id);

    item.armor_type = armor_type;
    item.armor_piece = piece;
    item.damage_reduce = damage_reduce_amounts[armor_type];
    item.durability = max_damages[armor_type] * 3 << piece;
    item.max_stack_size = 1;

    item_list[id + 256] = item;

    return item;
}