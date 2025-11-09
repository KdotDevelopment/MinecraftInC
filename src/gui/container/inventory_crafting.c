#include <gui/container/inventory_crafting.h>

#include <string.h>

inventory_t inventory_crafting_create(screen_t *screen, int x, int y) {
    inventory_t inventory = { 0 };
    memset(&inventory, 0, sizeof(inventory_t));

    inventory.size = x * y;
    inventory.host_screen = screen;
    inventory.stack_limit = 64;

    inventory.get_slot = inventory_crafting_get_slot;
    inventory.get_name = inventory_crafting_get_name;
    inventory.remove_item = inventory_crafting_remove_item;
    inventory.set_slot = inventory_crafting_set_slot;
    inventory.on_changed = inventory_crafting_on_changed;

    return inventory;
}

item_stack_t inventory_crafting_get_slot(inventory_t *inventory, int index) {
    return inventory->inv[index];
}

char *inventory_crafting_get_name() {
    return "Crafting";
}

item_stack_t inventory_crafting_remove_item(inventory_t *inventory, int index, int amount) {
    if(inventory->inv[index].item_id != 0) {
        if(inventory->inv[index].stack_size <= amount) {
            item_stack_t item = inventory->inv[index];
            inventory->inv[index] = item_stack_create(0, 0, 0);
            inventory->host_screen->update_crafting(inventory->host_screen);
            return item;
        }else {
            item_stack_t item = item_stack_split(&inventory->inv[index], amount);
            if(inventory->inv[index].stack_size == 0) {
                inventory->inv[index] = item_stack_create(0, 0, 0);
            }
            inventory->host_screen->update_crafting(inventory->host_screen);
            return item;
        }
    }else {
        return item_stack_create(0, 0, 0);
    }
}

void inventory_crafting_set_slot(inventory_t *inventory, int index, item_stack_t item) {
    inventory->inv[index] = item;
    inventory->host_screen->update_crafting(inventory->host_screen);
}

void inventory_crafting_on_changed(inventory_t *inventory) {
    return;
}