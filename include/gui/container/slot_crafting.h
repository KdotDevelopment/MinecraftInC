#pragma once

#include <gui/container/slot.h>
#include <gui/screen.h>

slot_t slot_crafting_create(screen_t *screen, inventory_t *crafting_matrix, inventory_t *inventory, int index, int x, int y);
uint8_t slot_crafting_can_put_item(slot_t *slot, item_stack_t item);
void slot_crafting_on_pickup(slot_t *slot);
