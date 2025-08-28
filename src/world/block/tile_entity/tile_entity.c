#include <world/block/tile_entity/tile_entity.h>

#include <nbt/nbt_base.h>
#include <world/world.h>

#include <string.h>

void tile_entity_create(tile_entity_t *tile_entity, world_t *world, int x, int y, int z) {
    memset(tile_entity, 0, sizeof(tile_entity_t));

    tile_entity->world = world;
    tile_entity->x = x;
    tile_entity->y = y;
    tile_entity->z = z;
    tile_entity->inventory_size = 0;

    tile_entity->update = tile_entity_update;
    tile_entity->on_inventory_changed = tile_entity_on_inventory_changed;
    tile_entity->read_nbt = tile_entity_read_nbt;
    tile_entity->write_nbt = tile_entity_write_nbt;
    tile_entity->get_name = tile_entity_get_name;
}

tile_entity_t tile_entity_load(nbt_base_t *nbt) {
    tile_entity_t tile_entity = { 0 };

    char *name = nbt_tag_compound_get_string(nbt, "id");

    if(strcmp(name, "Chest") == 0) {
        tile_entity_chest_read_nbt(&tile_entity, nbt);
    }

    if(strcmp(name, "Furnace") == 0) {
        tile_entity_furnace_read_nbt(&tile_entity, nbt);
    }

    return tile_entity;
}

void tile_entity_read_nbt(tile_entity_t *tile_entity, nbt_base_t *nbt) {
    tile_entity->x = nbt_tag_compound_get_int(nbt, "x");
    tile_entity->y = nbt_tag_compound_get_int(nbt, "y");
    tile_entity->z = nbt_tag_compound_get_int(nbt, "z");
}

void tile_entity_write_nbt(tile_entity_t *tile_entity, nbt_base_t *nbt) {
    nbt_tag_compound_set_string(nbt, "id", tile_entity->get_name());
    nbt_tag_compound_set_int(nbt, "x", tile_entity->x);
    nbt_tag_compound_set_int(nbt, "y", tile_entity->y);
    nbt_tag_compound_set_int(nbt, "z", tile_entity->z);
}

char *tile_entity_get_name() {
    return "";
}

void tile_entity_update(tile_entity_t *tile_entity) {
    return;
}

void tile_entity_on_inventory_changed(tile_entity_t *tile_entity) {
    return;
}