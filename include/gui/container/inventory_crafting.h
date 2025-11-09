#pragma once

#include <gui/container/inventory.h>
#include <gui/screen.h>

inventory_t inventory_crafting_create(screen_t *screen, int x, int y);
item_stack_t inventory_crafting_get_slot(inventory_t *inventory, int index);
char *inventory_crafting_get_name();
item_stack_t inventory_crafting_remove_item(inventory_t *inventory, int index, int amount);
void inventory_crafting_set_slot(inventory_t *inventory, int index, item_stack_t item);
void inventory_crafting_on_changed(inventory_t *inventory);