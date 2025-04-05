#pragma once

#include <level/entity_map_slot.h>
#include <entity/entity.h>
#include <renderer/frustum.h>

#include <stdint.h>

typedef struct entity_map_s {
    int width;
    int height;
    int depth;
    entity_map_slot_t temp_slot_1;
    entity_map_slot_t temp_slot_2;
    entity_t ***entities; // array of arraylists of entity pointers
    entity_t **all; // arraylist of entities
    entity_t **temp; // arraylist of entities
} entity_map_t;

void entity_map_create(entity_map_t *map, int width, int height, int depth);
void entity_map_insert(entity_map_t *map, entity_t *entity);
void entity_map_remove(entity_map_t *map, entity_t *entity);
void entity_map_moved(entity_map_t *map, entity_t *entity);
//pointer to arraylist
entity_t ***entity_map_get_entities(entity_map_t *map, entity_t *entity, float x0, float y0, float z0, float x1, float y1, float z1);
void entity_map_clear_non_creative_mode_entities(entity_map_t *map);
void entity_map_clear(entity_map_t *map);
void entity_map_tick_all(entity_map_t *map);
void entity_map_render(entity_map_t *map, vec3_t vector, frustum_t *frustum, textures_t *textures, float delta);
void entity_map_destroy(entity_map_t *map);