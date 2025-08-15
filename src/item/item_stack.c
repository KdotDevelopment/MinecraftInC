#include <item/item_stack.h>

item_stack_t item_stack_create(uint8_t block_item_id, uint8_t stack_size, int16_t item_damage) {
    item_stack_t item_stack = { 0 };

    item_stack.item_id = block_item_id;
    item_stack.stack_size = stack_size;
    item_stack.item_damage = item_damage;

    return item_stack;
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
