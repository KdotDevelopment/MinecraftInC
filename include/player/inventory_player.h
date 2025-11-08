#pragma once

#include <item/item_stack.h>
#include <gui/container/inventory.h>
#include <world/block/block.h>

#include <stdint.h>

inventory_t inventory_player_create(entity_t *player);
item_stack_t inventory_player_get_selected(inventory_t *inventory);
int inventory_player_get_empty_slot(inventory_t *inventory);
void inventory_player_get_hotbar_slot(inventory_t *inventory, int item_id);
uint8_t inventory_player_consume_item(inventory_t *inventory, int item_id);
uint8_t inventory_player_add_item(inventory_t *inventory, item_stack_t *item);
item_stack_t inventory_player_remove_item(inventory_t *inventory, int slot, int amount);
void inventory_player_set_slot(inventory_t *inventory, int slot, item_stack_t item);
item_stack_t inventory_player_get_slot(inventory_t *inventory, int slot);
char *inventory_player_get_name();
int inventory_player_get_armor_value(inventory_t *inventory);
void inventory_player_on_changed(inventory_t *inventory);
void inventory_player_drop_items(inventory_t *inventory);