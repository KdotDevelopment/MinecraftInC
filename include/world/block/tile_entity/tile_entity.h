#pragma once

#include <item/item_stack.h>
#include <gui/container/inventory.h>

struct world_s;

typedef struct tile_entity_s {
    struct world_s *world;
    uint8_t inventory_size;
    inventory_t inventory;
    int x;
    int y;
    int z;

    union {
        // Chest
        struct {
            item_stack_t chest_contents[36];
            inventory_t chest_inventory; // specifically to accomodate double chests
        };
        // Furnace
        struct {
            item_stack_t furnace_contents[3];
            int cook_time;
            int burn_time;
            int current_item_burn_time;
        };
    };

    void (*update)(struct tile_entity_s *tile_entity);
    void (*on_inventory_changed)(struct tile_entity_s *tile_entity);
    void (*read_nbt)(struct tile_entity_s *tile_entity, nbt_base_t *nbt);
    void (*write_nbt)(struct tile_entity_s *tile_entity, nbt_base_t *nbt);
    char *(*get_name)();
} tile_entity_t;

void tile_entity_create(tile_entity_t *tile_entity, struct world_s *world, int x, int y, int z);
tile_entity_t tile_entity_load(nbt_base_t *nbt);
void tile_entity_read_nbt(tile_entity_t *tile_entity, nbt_base_t *nbt);
void tile_entity_write_nbt(tile_entity_t *tile_entity, nbt_base_t *nbt);
char *tile_entity_get_name();
void tile_entity_update(tile_entity_t *tile_entity);
void tile_entity_on_inventory_changed(tile_entity_t *tile_entity);