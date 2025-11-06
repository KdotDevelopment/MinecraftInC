#include <player/inventory_player.h>

#include <item/item.h>
#include <world/block/blocks.h>
#include <session_data.h>

#include <stdlib.h>

inventory_t inventory_player_create(entity_t *player) {
    inventory_t inventory = { 0 };

    inventory.selected = 0;
    inventory.size = 40;
    inventory.stack_limit = 64;
    inventory.host = player;

    for(int i = 0; i < 36; i++) {
        inventory.inv[i] = item_stack_create(0, 0, 0);
    }
    for(int i = 0; i < 4; i++) {
        inventory.armor[i] = item_stack_create(0, 0, 0);
    }

    inventory.get_name = inventory_player_get_name;
    inventory.get_slot = inventory_player_get_slot;
    inventory.remove_item = inventory_player_remove_item;
    inventory.on_changed = inventory_player_on_changed;

    return inventory;
}

item_stack_t inventory_player_get_selected(inventory_t *inventory) {
    return inventory->inv[inventory->selected];
}

int get_slot(inventory_t *inventory, uint8_t block_id) {
    for(int i = 0; i < 36; i++) {
        if(inventory->inv[i].item_id != 0 && inventory->inv[i].item_id == block_id) {
            return i;
        }
    }

    return -1;
}

int inventory_player_get_empty_slot(inventory_t *inventory) {
    for(int i = 0; i < 36; i++) {
        if(inventory->inv[i].item_id == 0) return i;
    }
    return -1;
}

void inventory_player_get_hotbar_slot(inventory_t *inventory, int item_id) {
    int slot = get_slot(inventory, item_id);
    if(slot >= 0 && slot < 9) { // in hotbar
        inventory->selected = slot;
    }
}

uint8_t inventory_player_consume_item(inventory_t *inventory, int item_id) {
    int slot = get_slot(inventory, item_id);
    if(slot < 0) return 0;
    if(--inventory->inv[slot].stack_size <= 0) {
        inventory->inv[slot] = item_stack_create(0, 0, 0);
    }
    return 1;
}

uint8_t inventory_player_add_item(inventory_t *inventory, item_stack_t *item) {
    if(item->item_damage == 0) {
        int remaining = item->stack_size;
        int id = item->item_id;

        int slot = -1;
        for(int i = 0; i < 36; i++) {
            if(inventory->inv[i].item_id == id) {
                int limit = item_list[id].max_stack_size;
                if(limit > 64) limit = 64;
                if(inventory->inv[i].stack_size < limit) {
                    slot = i;
                    break;
                }
            }
        }

        if(slot < 0) {
            slot = inventory_player_get_empty_slot(inventory);
        }

        if(slot >= 0) {
            if(inventory->inv[slot].item_id == 0) {
                inventory->inv[slot] = item_stack_create(id, 0, 0);
            }

            int limit = item_list[inventory->inv[slot].item_id].max_stack_size;
            if(limit > 64) limit = 64;
            int space = limit - inventory->inv[slot].stack_size;
            if(space < 0) space = 0;
            int can_move = remaining < space ? remaining : space;

            if(can_move > 0) {
                inventory->inv[slot].stack_size += can_move;
                inventory->inv[slot].animations_to_go = 5;
                remaining -= can_move;
            }

            item->stack_size = remaining;
            if(item->stack_size == 0) return 1;
        } else {
            item->stack_size = remaining;
        }
    }

    int empty = inventory_player_get_empty_slot(inventory);
    if(empty >= 0) {
        inventory->inv[empty] = *item;
        inventory->inv[empty].animations_to_go = 5;
        return 1;
    }

    return 0;
}

item_stack_t inventory_player_remove_item(inventory_t *inventory, int slot, int amount) {
    item_stack_t *arr = inventory->inv;
    if(slot >= 36) {
        arr = inventory->armor;
        slot -= 36;
    }

    item_stack_t current = arr[slot];
    if(current.item_id == 0) return item_stack_create(0, 0, 0);

    if(current.stack_size <= amount) {
        item_stack_t taken = current;
        arr[slot] = item_stack_create(0, 0, 0);
        return taken;
    }else {
        item_stack_t taken = item_stack_split(&current, amount);
        if(current.stack_size == 0) {
            arr[slot] = item_stack_create(0, 0, 0);
        }
        return taken;
    }
}

void inventory_player_set_slot(inventory_t *inventory, int slot, item_stack_t item) {
    item_stack_t *arr = inventory->inv;
    if(slot >= 36) {
        arr = inventory->armor;
        slot -= 36;
    }

    arr[slot] = item;
}

item_stack_t inventory_player_get_slot(inventory_t *inventory, int slot) {
    item_stack_t *arr = inventory->inv;
    if(slot >= 36) {
        arr = inventory->armor;
        slot -= 36;
    }

    return arr[slot];
}

char *inventory_player_get_name() {
    return "Inventory";
}

int player_inventory_get_armor_value(inventory_t *inventory) {
    int total_reduced = 0;
    int remaining_durability = 0;
    int total_durability = 0;

    for(int i = 0; i < 4; i++) {
        item_stack_t item_stack = inventory->armor[i];
        if(item_stack.item_id == 0) continue;

        item_t item = item_list[item_stack.item_id];
        if(!item.is_armor) continue;

        int max = item.durability;
        int remaining = max - item_stack.item_damage;

        remaining_durability += remaining;
        total_durability += max;
        total_reduced += item.damage_reduce;
    }

    if(total_durability == 0) return 0; // no armor equipped

    return (total_reduced - 1) * remaining_durability / total_durability + 1;
}

void inventory_player_on_changed(inventory_t *inventory) {
    return;
}

void inventory_player_drop_items(inventory_t *inventory) {
    for(int i = 0; i < 36; i++) {
        if(inventory->inv[i].item_id != 0) {
            // player drop randomly
            inventory->inv[i] = item_stack_create(0, 0, 0);
        }
    }
    for(int i = 0; i < 4; i++) {
        if(inventory->armor[i].item_id != 0) {
            // player drop randomly
            inventory->armor[i] = item_stack_create(0, 0, 0);
        }
    }
}