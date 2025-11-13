#pragma once

#include <gui/container/inventory.h>

inventory_t inventory_double_chest_create(char *name, inventory_t *upper_chest, inventory_t *lower_chest);
item_stack_t inventory_double_chest_get_slot(inventory_t *inventory, int index);
char *inventory_double_chest_get_name();
item_stack_t inventory_double_chest_remove_item(inventory_t *inventory, int index, int amount);
void inventory_double_chest_set_slot(inventory_t *inventory, int index, item_stack_t item);
void inventory_double_chest_on_changed(inventory_t *inventory);