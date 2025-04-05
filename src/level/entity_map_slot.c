#include <level/entity_map_slot.h>
#include <level/entity_map.h>

#include <util/array_list.h>

void entity_map_slot_create(entity_map_slot_t *slot, struct entity_map_s *map) {
    slot->x = 0;
    slot->y = 0;
    slot->z = 0;
    slot->map = map;
}

void entity_map_slot_init(entity_map_slot_t *slot, float x, float y, float z) {
    slot->x = (int)x / 16;
    slot->y = (int)y / 16;
    slot->z = (int)z / 16;
    if(slot->x < 0) {
        slot->x = 0;
    }
    if(slot->y < 0) {
        slot->y = 0;
    }
    if(slot->z < 0) {
        slot->z = 0;
    }
    if(slot->x >= slot->map->width) {
        slot->x = slot->map->width - 1;
    }
    if(slot->y >= slot->map->height) {
        slot->y = slot->map->height - 1;
    }
    if(slot->z >= slot->map->depth) {
        slot->z = slot->map->depth - 1;
    }
}

void entity_map_slot_add(entity_map_slot_t *slot, struct entity_s *entity) {
    if(slot->x < 0 || slot->y < 0 || slot->z < 0) return;
    entity_t **list = slot->map->entities[(slot->y * slot->map->depth + slot->z) * slot->map->width + slot->x];
    list = array_list_push(list, &entity);
    slot->map->entities[(slot->y * slot->map->depth + slot->z) * slot->map->width + slot->x] = list;
}

void entity_map_slot_remove(entity_map_slot_t *slot, struct entity_s *entity) {
    if(slot->x < 0 || slot->y < 0 || slot->z < 0) return;
    entity_t **list = slot->map->entities[(slot->y * slot->map->depth + slot->z) * slot->map->width + slot->x];
    int index = array_list_index_of(list, &entity);
    list = array_list_remove(list, index);
    slot->map->entities[(slot->y * slot->map->depth + slot->z) * slot->map->width + slot->x] = list;
}

uint8_t entity_map_equals(entity_map_slot_t *slot1, entity_map_slot_t *slot2) {
    return slot1->x == slot2->x && slot1->y == slot2->y && slot1->z == slot2->z;
}