#include <world/block/tile_entity/tile_entity_chest.h>

#include <nbt/nbt_tag_list.h>
#include <util/array_list.h>
#include <world/world.h>

#include <string.h>

void tile_entity_chest_create(tile_entity_t *tile_entity, world_t *world, int x, int y, int z) {
    tile_entity_create(tile_entity, world, x, y, z);

    tile_entity->update = tile_entity_update;
    tile_entity->on_inventory_changed = tile_entity_chest_on_changed;
    tile_entity->read_nbt = tile_entity_chest_read_nbt;
    tile_entity->write_nbt = tile_entity_chest_write_nbt;
    tile_entity->get_name = tile_entity_chest_get_name;
    tile_entity->inventory_size = 27;

    memset(&tile_entity->inventory, 0, sizeof(inventory_t));
    tile_entity->inventory.size = tile_entity->inventory_size;
    tile_entity->inventory.stack_limit = 64;
    tile_entity->inventory.host_tile_entity = tile_entity;

    tile_entity->inventory.get_slot = tile_entity_chest_get_item;
    tile_entity->inventory.set_slot = tile_entity_chest_set_slot_contents;
    tile_entity->inventory.remove_item = tile_entity_chest_decr_stack_size;
    tile_entity->inventory.get_name = tile_entity_chest_get_name;
    tile_entity->inventory.on_changed = tile_entity_chest_on_inventory_changed;
}

item_stack_t tile_entity_chest_get_item(inventory_t *inventory, int slot) {
    if(inventory == NULL || inventory->host_tile_entity == NULL) {
        return item_stack_create(0, 0, 0);
    }

    tile_entity_t *tile_entity = inventory->host_tile_entity;
    if(slot < 0 || slot >= tile_entity->inventory_size) {
        return item_stack_create(0, 0, 0);
    }
    return tile_entity->chest_contents[slot];
}

item_stack_t tile_entity_chest_decr_stack_size(inventory_t *inventory, int slot, int amount) {
    if(inventory == NULL || inventory->host_tile_entity == NULL) {
        return item_stack_create(0, 0, 0);
    }

    tile_entity_t *tile_entity = inventory->host_tile_entity;
    if(slot < 0 || slot >= tile_entity->inventory_size) {
        return item_stack_create(0, 0, 0);
    }
    item_stack_t *item_stack = &tile_entity->chest_contents[slot];
    if(item_stack->stack_size == 0) {
        return item_stack_create(0, 0, 0);
    }

    if(item_stack->stack_size <= amount) {
        item_stack_t removed = *item_stack;
        tile_entity->chest_contents[slot] = item_stack_create(0, 0, 0);
        return removed;
    }

    item_stack_t removed = item_stack_split(item_stack, amount);
    if(item_stack->stack_size == 0) {
        tile_entity->chest_contents[slot] = item_stack_create(0, 0, 0);
    }
    return removed;
}

void tile_entity_chest_set_slot_contents(inventory_t *inventory, int slot, item_stack_t item) {
    if(inventory == NULL || inventory->host_tile_entity == NULL) {
        return;
    }

    tile_entity_t *tile_entity = inventory->host_tile_entity;
    if(slot < 0 || slot >= tile_entity->inventory_size) {
        return;
    }
    if(item.stack_size > 64) {
        item.stack_size = 64;
    }
    tile_entity->chest_contents[slot] = item;
}

void tile_entity_chest_read_nbt(tile_entity_t *tile_entity, nbt_base_t *nbt) {
    tile_entity_read_nbt(tile_entity, nbt);
    
    nbt_base_t items = nbt_tag_compound_get_tag_list(nbt, "Items");
    memset(&tile_entity->inventory, 0, sizeof(inventory_t));
    tile_entity->inventory_size = 27;
    
    for(int i = 0; i < array_list_length(items.tag_array); i++) {
        nbt_base_t *item = nbt_tag_list_get_tag(&items, i);
        uint8_t slot = nbt_tag_compound_get_byte(item, "Slot") & 0xFF;
        if(slot >= 0 && slot < tile_entity->inventory_size) {
            tile_entity->chest_contents[slot] = item_stack_from_nbt(item);
        }
    }

    tile_entity->update = tile_entity_update;
    tile_entity->on_inventory_changed = tile_entity_chest_on_changed;
    tile_entity->read_nbt = tile_entity_chest_read_nbt;
    tile_entity->write_nbt = tile_entity_chest_write_nbt;
    tile_entity->get_name = tile_entity_chest_get_name;

    tile_entity->inventory.size = tile_entity->inventory_size;
    tile_entity->inventory.stack_limit = 64;
    tile_entity->inventory.host_tile_entity = tile_entity;

    tile_entity->inventory.get_slot = tile_entity_chest_get_item;
    tile_entity->inventory.set_slot = tile_entity_chest_set_slot_contents;
    tile_entity->inventory.remove_item = tile_entity_chest_decr_stack_size;
    tile_entity->inventory.get_name = tile_entity_chest_get_name;
    tile_entity->inventory.on_changed = tile_entity_chest_on_inventory_changed;
}

void tile_entity_chest_write_nbt(tile_entity_t *tile_entity, nbt_base_t *nbt) {
    tile_entity_write_nbt(tile_entity, nbt);

    nbt_base_t tag_list = nbt_tag_list_create();

    for(int i = 0; i < tile_entity->inventory_size; i++) {
        if(tile_entity->chest_contents[i].item_id != 0) {
            nbt_base_t item = nbt_tag_compound_create();
            nbt_tag_compound_set_byte(&item, "Slot", i);
            item_stack_write_nbt(&tile_entity->chest_contents[i], &item);
            nbt_tag_list_set_tag(&tag_list, &item);
        }
    }

    nbt_tag_compound_set_tag(nbt, "Items", &tag_list);
}

char *tile_entity_chest_get_name() {
    return "Chest";
}

void tile_entity_chest_on_inventory_changed(inventory_t *inventory) {
    if(inventory == NULL) {
        return;
    }

    tile_entity_t *tile_entity = inventory->host_tile_entity;
    if(tile_entity == NULL || tile_entity->world == NULL) {
        return;
    }

    world_update_chunk(tile_entity->world, tile_entity->x, tile_entity->y, tile_entity->z);
}

void tile_entity_chest_on_changed(tile_entity_t *tile_entity) {
    world_update_chunk(tile_entity->world, tile_entity->x, tile_entity->y, tile_entity->z);
}