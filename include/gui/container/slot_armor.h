#pragma once

#include <gui/container/slot.h>
#include <gui/screen.h>

slot_t slot_armor_create(screen_t *screen, inventory_t *inventory, int index, int x, int y, int armor_type);
uint8_t slot_armor_can_put_item(slot_t *slot, item_stack_t item);
int slot_armor_get_texture(slot_t *slot);