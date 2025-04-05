#pragma once

#include <stdint.h>

struct entity_s;

typedef struct entity_map_slot_s {
    int x;
    int y;
    int z;
    struct entity_map_s *map;
} entity_map_slot_t;

void entity_map_slot_create(entity_map_slot_t *slot, struct entity_map_s *map);
void entity_map_slot_init(entity_map_slot_t *slot, float x, float y, float z);
void entity_map_slot_add(entity_map_slot_t *slot, struct entity_s *entity);
void entity_map_slot_remove(entity_map_slot_t *slot, struct entity_s *entity);
uint8_t entity_map_equals(entity_map_slot_t *slot1, entity_map_slot_t *slot2);