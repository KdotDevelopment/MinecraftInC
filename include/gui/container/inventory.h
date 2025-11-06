#pragma once

#include <entity/entity.h>
#include <item/item_stack.h>

typedef struct inventory_s {
    item_stack_t inv[36];
    item_stack_t armor[4]; // only for player inventories
    int selected;
    int size;
    int stack_limit;
    entity_t *host; // typically the player

    char *(*get_name)();
    item_stack_t (*get_slot)(struct inventory_s *inventory, int slot);
    item_stack_t (*remove_item)(struct inventory_s *inventory, int slot, int amount);
    void (*on_changed)(struct inventory_s *inventory);
} inventory_t;