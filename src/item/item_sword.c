#include <item/item_sword.h>

item_t item_sword_create(uint8_t id, uint8_t texture_id, uint8_t sword_type) {
    item_t item = item_create(id, texture_id);

    item.max_stack_size = 1;
    item.durability = 32 << sword_type;
    item.weapon_damage = 4 + (sword_type * 2);

    item.get_strength_against_block = item_sword_get_strength_against_block;
    item.hit_entity = item_sword_hit_entity;
    item.on_block_destroy = item_sword_on_block_destroy;
    item.get_damage_against_entity = item_sword_get_damage_against_entity;

    item_list[id + 256] = item;
    
    return item;
}

float item_sword_get_strength_against_block(item_t *item, block_t *block) {
    return 1.5;
}

void item_sword_hit_entity(item_t *item, item_stack_t *item_stack) {
    item_stack_damage(item_stack, 1);
}

void item_sword_on_block_destroy(item_t *item, item_stack_t *item_stack) {
    item_stack_damage(item_stack, 1);
}

int item_sword_get_damage_against_entity(item_t *item) {
    return item->weapon_damage;
}