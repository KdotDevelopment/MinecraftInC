#include <gui/container/inventory_double_chest.h>

#include <string.h>

inventory_t inventory_double_chest_create(char *name, inventory_t *upper_chest, inventory_t *lower_chest) {
    inventory_t inventory = { 0 };
    memset(&inventory, 0, sizeof(inventory_t));

    inventory.size = upper_chest->size + lower_chest->size;
    strncpy(inventory.name, name, 27);
    inventory.stack_limit = upper_chest->stack_limit;
    inventory.host_tile_entity = upper_chest->host_tile_entity != NULL ? upper_chest->host_tile_entity : lower_chest->host_tile_entity;
    inventory.upper_chest = upper_chest;
    inventory.lower_chest = lower_chest;

    inventory.get_slot = inventory_double_chest_get_slot;
    inventory.get_name = inventory_double_chest_get_name;
    inventory.remove_item = inventory_double_chest_remove_item;
    inventory.set_slot = inventory_double_chest_set_slot;
    inventory.on_changed = inventory_double_chest_on_changed;

    return inventory;
}

item_stack_t inventory_double_chest_get_slot(inventory_t *inventory, int index) {
    if(index >= inventory->upper_chest->size) {
        return inventory->lower_chest->get_slot(inventory->lower_chest, index - inventory->upper_chest->size);
    }else {
        return inventory->upper_chest->get_slot(inventory->upper_chest, index);
    }
}

char *inventory_double_chest_get_name() {
    return "Large chest";
}

item_stack_t inventory_double_chest_remove_item(inventory_t *inventory, int index, int amount) {
    if(index >= inventory->upper_chest->size) {
        return inventory->lower_chest->remove_item(inventory->lower_chest, index - inventory->upper_chest->size, amount);
    }else {
        return inventory->upper_chest->remove_item(inventory->upper_chest, index, amount);
    }
}

void inventory_double_chest_set_slot(inventory_t *inventory, int index, item_stack_t item) {
    if(index >= inventory->upper_chest->size) {
        inventory->lower_chest->set_slot(inventory->lower_chest, index - inventory->upper_chest->size, item);
    }else {
        inventory->upper_chest->set_slot(inventory->upper_chest, index, item);
    }
}

void inventory_double_chest_on_changed(inventory_t *inventory) {
    inventory->upper_chest->on_changed(inventory->upper_chest);
    inventory->lower_chest->on_changed(inventory->lower_chest);
}