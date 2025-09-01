#include <item/item_stack.h>

#include <item/item.h>

item_stack_t item_stack_create(uint8_t item_id, uint8_t stack_size, int16_t item_damage) {
    item_stack_t item_stack = { 0 };

    item_stack.item_id = item_id;
    item_stack.stack_size = stack_size;
    item_stack.item_damage = item_damage;

    return item_stack;
}

item_stack_t item_stack_from_nbt(nbt_base_t *nbt) {
    item_stack_t item_stack = { 0 };

    item_stack.item_id = nbt_tag_compound_get_short(nbt, "id");
    item_stack.stack_size = nbt_tag_compound_get_byte(nbt, "Count") & 0xFF;
    item_stack.item_damage = nbt_tag_compound_get_short(nbt, "Damage");

    return item_stack;
}

nbt_base_t *item_stack_write_nbt(item_stack_t *item_stack, nbt_base_t *nbt) {
    nbt_tag_compound_set_short(nbt, "id", item_stack->item_id);
    nbt_tag_compound_set_byte(nbt, "Count", item_stack->stack_size);
    nbt_tag_compound_set_short(nbt, "Damage", item_stack->item_damage);
    
    return nbt;
}

item_stack_t item_stack_split(item_stack_t *item_stack, uint8_t split_size) {
    item_stack->stack_size -= split_size;

    item_stack_t new_item_stack = item_stack_create(item_stack->item_id, split_size, item_stack->item_damage);

    return new_item_stack;
}

int16_t item_stack_get_max_damage(item_stack_t *item_stack) {
    return item_list[item_stack->item_id].durability;
}

void item_stack_damage(item_stack_t *item_stack, int16_t damage) {
    item_stack->item_damage += damage;

    if(item_stack->item_damage > item_stack_get_max_damage(item_stack)) {
        item_stack->stack_size--;
        if(item_stack->stack_size < 0) item_stack->stack_size = 0;
        item_stack->item_damage = 0;
    }
}
