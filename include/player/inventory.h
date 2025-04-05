#pragma once

#include <level/block/block.h>

#include <stdint.h>

typedef struct {
    int slots[9];
    int counts[9];
    int pop_times[9];
    int selected;
} inventory_t;

inventory_t inventory_create();
uint8_t inventory_get_selected(inventory_t *inventory);
void inventory_grab_texture(inventory_t *inventory, int block_id);
void inventory_swap_paint(inventory_t *inventory, int slot);
void inventory_replace_slot(inventory_t *inventory, int block_id);
void inventory_replace_slot_with_block(inventory_t *inventory, block_t *block);
uint8_t inventory_add_item(inventory_t *inventory, int block_id);
void inventory_tick(inventory_t *inventory);
uint8_t inventory_remove_item(inventory_t *inventory, int block_id);