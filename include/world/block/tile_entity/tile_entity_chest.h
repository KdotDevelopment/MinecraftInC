#pragma once

#include <nbt/nbt_tag_compound.h>
#include <world/block/tile_entity/tile_entity.h>

struct world_s;

void tile_entity_chest_create(tile_entity_t *tile_entity, struct world_s *world, int x, int y, int z);
item_stack_t tile_entity_chest_get_item(inventory_t *inventory, int slot);
item_stack_t tile_entity_chest_decr_stack_size(inventory_t *inventory, int slot, int amount);
void tile_entity_chest_set_slot_contents(inventory_t *inventory, int slot, item_stack_t item);
void tile_entity_chest_read_nbt(tile_entity_t *tile_entity, nbt_base_t *nbt);
void tile_entity_chest_write_nbt(tile_entity_t *tile_entity, nbt_base_t *nbt);
char *tile_entity_chest_get_name();
void tile_entity_chest_on_inventory_changed(inventory_t *inventory);
void tile_entity_chest_on_changed(tile_entity_t *tile_entity);