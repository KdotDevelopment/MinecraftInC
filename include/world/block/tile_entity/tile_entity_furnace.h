#pragma once

#include <nbt/nbt_tag_compound.h>
#include <world/block/tile_entity/tile_entity.h>

struct world_s;

void tile_entity_furnace_create(tile_entity_t *tile_entity, struct world_s *world, int x, int y, int z);
item_stack_t tile_entity_furnace_get_item(inventory_t *inventory, int slot);
item_stack_t tile_entity_furnace_decr_stack_size(inventory_t *inventory, int slot, int amount);
void tile_entity_furnace_set_slot_contents(inventory_t *inventory, int slot, item_stack_t item);
void tile_entity_furnace_read_nbt(tile_entity_t *tile_entity, nbt_base_t *nbt);
void tile_entity_furnace_write_nbt(tile_entity_t *tile_entity, nbt_base_t *nbt);
char *tile_entity_furnace_get_name();
int tile_entity_furnace_get_cook_progress_scaled(tile_entity_t *tile_entity, int scale);
int tile_entity_furnace_get_burn_time_remaining_scaled(tile_entity_t *tile_entity, int scale);
uint8_t tile_entity_furnace_is_burning(tile_entity_t *tile_entity);
void tile_entity_furnace_update(tile_entity_t *tile_entity);
uint8_t tile_entity_furnace_can_smelt(tile_entity_t *tile_entity);
int16_t tile_entity_furnace_smelt_item(int16_t item_id);
int tile_entity_furnace_get_burn_time(item_stack_t *item_stack);
void tile_entity_furnace_on_inventory_changed(inventory_t *inventory);
void tile_entity_furnace_on_changed(tile_entity_t *tile_entity);