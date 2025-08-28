#include <world/block/tile_entity/tile_entity_chest.h>

#include <nbt/nbt_tag_list.h>

item_stack_t *tile_entity_chest_get_item(tile_entity_t *tile_entity, uint8_t slot) {
    return &tile_entity->chest_contents[slot];
}

item_stack_t *tile_entity_chest_decr_stack_size(tile_entity_t *tile_entity, uint8_t slot, uint8_t amount) {
    item_stack_t *item_stack = &tile_entity->chest_contents[slot];
    if(item_stack->stack_size == 0) {
        return (void *)0;
    }

    if(item_stack->stack_size <= amount) {
        item_stack_t item_stack_blank = { 0 };
        tile_entity->chest_contents[slot] = item_stack_blank;
        return &tile_entity->chest_contents[slot];
    }

    item_stack_split(item_stack, amount);
    if(item_stack->stack_size == 0) {
        item_stack_t item_stack_blank = { 0 };
        tile_entity->chest_contents[slot] = item_stack_blank;
    }
    return &tile_entity->chest_contents[slot];
}

void tile_entity_chest_set_slot_contents(tile_entity_t *tile_entity, uint8_t slot, item_stack_t item) {
    if(item.stack_size > 64) {
        item.stack_size = 64;
    }
    tile_entity->chest_contents[slot] = item;
}

void tile_entity_chest_read_nbt(tile_entity_t *tile_entity, nbt_base_t *nbt) {
    tile_entity_read_nbt(tile_entity, nbt);
    
    nbt_base_t *items = nbt_tag_compound_get_tag_list(nbt, "Items");
    tile_entity->inventory_size = 27;
    
    for(int i = 0; i < nbt_tag_list_get_count(items); i++) {
        nbt_base_t *item = nbt_tag_list_get_tag(items, i);
        uint8_t slot = nbt_tag_compound_get_byte(item, "Slot") & 0xFF;
        if(slot >= 0 && slot < tile_entity->inventory_size) {
            tile_entity->chest_contents[slot] = item_stack_from_nbt(item);
        }
    }

    tile_entity->update = tile_entity_update;
    tile_entity->on_inventory_changed = tile_entity_chest_on_inventory_changed;
    tile_entity->read_nbt = tile_entity_chest_read_nbt;
    tile_entity->write_nbt = tile_entity_chest_write_nbt;
    tile_entity->get_name = tile_entity_chest_get_name;
}

void tile_entity_chest_write_nbt(tile_entity_t *tile_entity, nbt_base_t *nbt) {
    tile_entity_write_nbt(tile_entity, nbt);

    nbt_base_t tag_list = { 0 };
    nbt_tag_list_create(&tag_list);

    for(int i = 0; i < tile_entity->inventory_size; i++) {
        if(tile_entity->chest_contents[i].item_id != 0) {
            nbt_base_t item = { 0 };
            nbt_tag_compound_create(&item);
            nbt_tag_compound_set_byte(&item, "Slot", i);
            tile_entity->chest_contents[i] = item_stack_from_nbt(&item);
            nbt_tag_list_set_tag(&tag_list, &item);
        }
    }

    nbt_tag_compound_set_tag(nbt, "Items", &tag_list);
}

char *tile_entity_chest_get_name() {
    return "Chest";
}

void tile_entity_chest_on_inventory_changed(tile_entity_t *tile_entity) {
    
}