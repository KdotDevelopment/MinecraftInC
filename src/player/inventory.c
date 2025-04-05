#include <player/inventory.h>
#include <level/block/blocks.h>
#include <session_data.h>

#include <stdlib.h>

inventory_t inventory_create() {
    inventory_t inventory = { 0 };
    inventory.selected = 0;

    for(int i = 0; i < 9; i++) {
        inventory.slots[i] = 0;
        inventory.counts[i] = 0;
    }

    return inventory;
}

uint8_t inventory_get_selected(inventory_t *inventory) {
    return inventory->slots[inventory->selected];
}

int get_slot(inventory_t *inventory, uint8_t block_id) {
    for(int i = 0; i < 9; i++) {
        if(inventory->slots[i] == block_id) {
            return i;
        }
    }

    return -1;
}

void inventory_grab_texture(inventory_t *inventory, int block_id) {
    int slot = get_slot(inventory, block_id);
    if(slot >= 0) {
        inventory->selected = slot;
        return;
    }
    if(block_id != blocks.air.id) {
        inventory_replace_slot_with_block(inventory, &block_list[session_allowed_blocks[block_id]]);
    }
}

void inventory_swap_paint(inventory_t *inventory, int slot) {
    slot = slot > 0 ? 1 : (slot < 0 ? -1 : slot);
    for(inventory->selected -= slot; inventory->selected < 0; inventory->selected += 9);
    while(inventory->selected >= 9) inventory->selected -= 9;
}

void inventory_replace_slot(inventory_t *inventory, int block_id) {
    if(block_id >= 0) {
        inventory_replace_slot_with_block(inventory, &block_list[session_allowed_blocks[block_id]]);
    }
}

void inventory_replace_slot_with_block(inventory_t *inventory, block_t *block) {
    if(block == NULL) return;
    int slot = get_slot(inventory, block->id);
    if(slot >= 0) {
        inventory->slots[slot] = inventory->slots[inventory->selected];
    }
    inventory->slots[inventory->selected] = block->id;
}

uint8_t inventory_add_item(inventory_t *inventory, int block_id) {
    int slot = get_slot(inventory, block_id);
    if(slot < 0) {
        slot = get_slot(inventory, -1);
    }

    if(slot < 0) return 0;
    if(inventory->counts[slot] >= 99) return 0;
    inventory->slots[slot] = block_id;
    inventory->counts[slot]++;
    inventory->pop_times[slot] = 5;
    return 1;
}

void inventory_tick(inventory_t *inventory) {
    for(int i = 0; i < 9; i++) {
        if(inventory->pop_times[i] > 0) {
            inventory->pop_times[i]--;
        }
    }
}

uint8_t inventory_remove_item(inventory_t *inventory, int block_id) {
    int slot = get_slot(inventory, block_id);
    if(slot >= 0) {
        inventory->counts[slot]--;
        if(inventory->counts[slot] <= 0) {
            inventory->slots[slot] = 0;
        }
        return 1;
    }else return 0;
}