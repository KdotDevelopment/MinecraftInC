#include <world/block/tile_entity/tile_entity_furnace.h>

#include <item/item.h>
#include <item/items.h>
#include <nbt/nbt_tag_list.h>
#include <util/array_list.h>
#include <world/block/blocks.h>
#include <world/material/materials.h>
#include <world/world.h>

#include <stdio.h>

item_stack_t *tile_entity_furnace_get_item(tile_entity_t *tile_entity, uint8_t slot) {
    return &tile_entity->furnace_contents[slot];
}

item_stack_t *tile_entity_furnace_decr_stack_size(tile_entity_t *tile_entity, uint8_t slot, uint8_t amount) {
    item_stack_t *item_stack = &tile_entity->furnace_contents[slot];
    if(item_stack->stack_size == 0) {
        return (void *)0;
    }

    if(item_stack->stack_size <= amount) {
        item_stack_t item_stack_blank = { 0 };
        tile_entity->furnace_contents[slot] = item_stack_blank;
        return &tile_entity->furnace_contents[slot];
    }

    item_stack_split(item_stack, amount);
    if(item_stack->stack_size == 0) {
        item_stack_t item_stack_blank = { 0 };
        tile_entity->furnace_contents[slot] = item_stack_blank;
    }
    return &tile_entity->furnace_contents[slot];
}

void tile_entity_furnace_set_slot_contents(tile_entity_t *tile_entity, uint8_t slot, item_stack_t item) {
    if(item.stack_size > 64) {
        item.stack_size = 64;
    }
    tile_entity->furnace_contents[slot] = item;
}

void tile_entity_furnace_read_nbt(tile_entity_t *tile_entity, nbt_base_t *nbt) {
    tile_entity_read_nbt(tile_entity, nbt);
    
    nbt_base_t items = nbt_tag_compound_get_tag_list(nbt, "Items");
    tile_entity->inventory_size = 27;

    for(int i = 0; i < array_list_length(items.tag_array); i++) {
        nbt_base_t *item = nbt_tag_list_get_tag(&items, i);
        uint8_t slot = nbt_tag_compound_get_byte(item, "Slot") & 0xFF;
        if(slot >= 0 && slot < tile_entity->inventory_size) {
            tile_entity->furnace_contents[slot] = item_stack_from_nbt(item);
        }
    }

    tile_entity->burn_time = nbt_tag_compound_get_short(nbt, "BurnTime");
    tile_entity->cook_time = nbt_tag_compound_get_short(nbt, "CookTime");
    tile_entity->current_item_burn_time = tile_entity_furnace_get_burn_time(&tile_entity->furnace_contents[1]);
    printf("Lit: %d / %d\n", tile_entity->burn_time, tile_entity->current_item_burn_time);

    tile_entity->update = tile_entity_furnace_update;
    tile_entity->on_inventory_changed = tile_entity_furnace_on_inventory_changed;
    tile_entity->read_nbt = tile_entity_furnace_read_nbt;
    tile_entity->write_nbt = tile_entity_furnace_write_nbt;
    tile_entity->get_name = tile_entity_furnace_get_name;
}

void tile_entity_furnace_write_nbt(tile_entity_t *tile_entity, nbt_base_t *nbt) {
    tile_entity_write_nbt(tile_entity, nbt);

    nbt_base_t tag_list = nbt_tag_list_create();

    nbt_tag_compound_set_short(nbt, "BurnTime", tile_entity->burn_time);
    nbt_tag_compound_set_short(nbt, "CookTime", tile_entity->cook_time);

    for(int i = 0; i < tile_entity->inventory_size; i++) {
        if(tile_entity->furnace_contents[i].item_id != 0) {
            nbt_base_t item = nbt_tag_compound_create();
            nbt_tag_compound_set_byte(&item, "Slot", i);
            tile_entity->furnace_contents[i] = item_stack_from_nbt(&item);
            nbt_tag_list_set_tag(&tag_list, &item);
        }
    }

    nbt_tag_compound_set_tag(nbt, "Items", &tag_list);
}

// In the original code, this class has a "getInventoryName" which returns "Chest", even in the Furnace class
// After confirming in-game, it appears it doesn't actually say "Chest"
char *tile_entity_furnace_get_name() {
    return "Furnace";
}

// In the original code, this has an unused integer argument
int tile_entity_furnace_get_cook_progress_scaled(tile_entity_t *tile_entity) {
    return (tile_entity->cook_time * 24) / 200;
}

// In the original code, this also has an unused integer argument
int tile_entity_furnace_get_burn_time_remaining_scaled(tile_entity_t *tile_entity) {
    return (tile_entity->burn_time * 12) / tile_entity->current_item_burn_time;
}

uint8_t tile_entity_furnace_is_burning(tile_entity_t *tile_entity) {
    return tile_entity->burn_time > 0;
}

void tile_entity_furnace_update(tile_entity_t *tile_entity) {
    uint8_t is_burning = tile_entity_furnace_is_burning(tile_entity);
    if(tile_entity->burn_time > 0) {
        tile_entity->burn_time--;
    }

    if(tile_entity->burn_time == 0 && tile_entity_furnace_can_smelt(tile_entity)) {
        tile_entity->current_item_burn_time = tile_entity->burn_time = tile_entity_furnace_get_burn_time(&tile_entity->furnace_contents[1]);
        if(tile_entity->burn_time > 0 && tile_entity->furnace_contents[1].item_id != 0) {
            tile_entity->furnace_contents[1].stack_size--;
            if(tile_entity->furnace_contents[1].stack_size == 0) {
                item_stack_t item_stack_blank = { 0 };
                tile_entity->furnace_contents[1] = item_stack_blank;
            }
        }
    }

    if(tile_entity_furnace_is_burning(tile_entity) && tile_entity_furnace_can_smelt(tile_entity)) {
        tile_entity->cook_time++;
        if(tile_entity->cook_time == 200) {
            tile_entity->cook_time = 0;
            if(tile_entity_furnace_can_smelt(tile_entity)) {
                uint8_t block_id = tile_entity_furnace_smelt_item(tile_entity->furnace_contents[0].item_id);
                if(tile_entity->furnace_contents[2].item_id == 0) {
                    tile_entity->furnace_contents[2] = item_stack_create(block_id, 1, 0);
                }else if(tile_entity->furnace_contents[2].item_id == block_id) {
                    tile_entity->furnace_contents[2].stack_size++;
                }

                tile_entity->furnace_contents[0].stack_size--;
                if(tile_entity->furnace_contents[0].stack_size <= 0) {
                    item_stack_t item_stack_blank = { 0 };
                    tile_entity->furnace_contents[0] = item_stack_blank;
                }
            }
        }
    }else {
        tile_entity->cook_time = 0;
    }

    if(is_burning != (tile_entity->burn_time > 0)) {
        is_burning = tile_entity_furnace_is_burning(tile_entity);
        int x = tile_entity->x;
        int y = tile_entity->y;
        int z = tile_entity->z;
        uint8_t metadata = world_get_block_metadata(tile_entity->world, x, y, z);
        tile_entity_t *new_tile_entity = world_get_tile_entity(tile_entity->world, x, y, z);
        if(is_burning) {
            world_set_block_with_update(tile_entity->world, x, y, z, BLOCK_FURNACE_LIT);
        }else {
            world_set_block_with_update(tile_entity->world, x, y, z, BLOCK_FURNACE);
        }

        world_set_block_metadata(tile_entity->world, x, y, z, metadata);
        world_set_tile_entity(tile_entity->world, x, y, z, new_tile_entity);
    }

    // world_update_tile_entity .....
}

uint8_t tile_entity_furnace_can_smelt(tile_entity_t *tile_entity) {
    if(tile_entity->furnace_contents[0].item_id == 0) {
        return 0;
    }

    int16_t item_id = tile_entity_furnace_smelt_item(tile_entity->furnace_contents[0].item_id);
    if(item_id < 0) return 0;
    if(tile_entity->furnace_contents[2].item_id == 0) return 1;
    if(tile_entity->furnace_contents[2].item_id != item_id) return 0;
    if(tile_entity->furnace_contents[2].stack_size < 64) {
        item_stack_t *item_stack = &tile_entity->furnace_contents[2];
        if(tile_entity->furnace_contents[2].stack_size < item_list[item_stack->item_id].max_stack_size) {
            return 1;
        }
    }

    return tile_entity->furnace_contents[2].stack_size < item_list[item_id].max_stack_size;
}

int16_t tile_entity_furnace_smelt_item(int16_t item_id) {
    if(item_id == blocks.iron_ore.id) return items.iron_ingot.id;
    if(item_id == blocks.gold_ore.id) return items.gold_ingot.id;
    if(item_id == blocks.diamond_ore.id) return items.diamond.id;
    if(item_id == blocks.sand.id) return blocks.glass.id;
    if(item_id == items.raw_pork.id) return items.cooked_pork.id;
    if(item_id == blocks.cobblestone.id) return blocks.stone.id;
    return -1;
}

int tile_entity_furnace_get_burn_time(item_stack_t *item_stack) {
    if(item_stack == NULL) return 0;
    int16_t item_id = item_stack->item_id;
    if(item_id < 256 && block_list[item_id].material == &materials.wood) return 300;
    if(item_id == items.stick.id) return 100;
    if(item_id == items.coal.id) return 1600;
    return 0;
}

void tile_entity_furnace_on_inventory_changed(tile_entity_t *tile_entity) {
    return;
}