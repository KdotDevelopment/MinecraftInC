#include <gui/container/inventory_crafting_result.h>

#include <string.h>

inventory_t inventory_crafting_result_create() {
    inventory_t inventory = { 0 };
    memset(&inventory, 0, sizeof(inventory_t));

    inventory.size = 1;
    inventory.stack_limit = 64;

    inventory.get_slot = inventory_crafting_result_get_slot;
    inventory.get_name = inventory_crafting_result_get_name;
    inventory.remove_item = inventory_crafting_result_remove_item;
    inventory.set_slot = inventory_crafting_result_set_slot;
    inventory.on_changed = inventory_crafting_result_on_changed;

    return inventory;
}

item_stack_t inventory_crafting_result_get_slot(inventory_t *inventory, int index) {
    return inventory->inv[index];
}

char *inventory_crafting_result_get_name() {
    return "Result";
}

item_stack_t inventory_crafting_result_remove_item(inventory_t *inventory, int index, int amount) {
    if(inventory->inv[index].item_id != 0) {
        item_stack_t item = inventory->inv[index];
        inventory->inv[index] = item_stack_create(0, 0, 0);
        return item;
    }else {
        return item_stack_create(0, 0, 0);
    }
}

void inventory_crafting_result_set_slot(inventory_t *inventory, int index, item_stack_t item) {
    inventory->inv[index] = item;
}

void inventory_crafting_result_on_changed(inventory_t *inventory) {
    return;
}