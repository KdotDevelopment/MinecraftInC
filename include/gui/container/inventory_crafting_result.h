#pragma once

#include <gui/container/inventory.h>
#include <gui/screen.h>

inventory_t inventory_crafting_result_create();
item_stack_t inventory_crafting_result_get_slot(inventory_t *inventory, int index);
char *inventory_crafting_result_get_name();
item_stack_t inventory_crafting_result_remove_item(inventory_t *inventory, int index, int amount);
void inventory_crafting_result_set_slot(inventory_t *inventory, int index, item_stack_t item);
void inventory_crafting_result_on_changed(inventory_t *inventory);