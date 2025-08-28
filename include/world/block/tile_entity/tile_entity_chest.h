#pragma once

#include <nbt/nbt_tag_compound.h>
#include <world/block/tile_entity/tile_entity.h>

item_stack_t *tile_entity_chest_get_item(tile_entity_t *tile_entity, uint8_t slot);
item_stack_t *tile_entity_chest_decr_stack_size(tile_entity_t *tile_entity, uint8_t slot, uint8_t amount);
void tile_entity_chest_set_slot_contents(tile_entity_t *tile_entity, uint8_t slot, item_stack_t item);
void tile_entity_chest_read_nbt(tile_entity_t *tile_entity, nbt_base_t *nbt);
void tile_entity_chest_write_nbt(tile_entity_t *tile_entity, nbt_base_t *nbt);
char *tile_entity_chest_get_name();
void tile_entity_chest_on_inventory_changed(tile_entity_t *tile_entity);