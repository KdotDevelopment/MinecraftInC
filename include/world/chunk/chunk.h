#pragma once

#include <entity/entity.h>
#include <nbt/nbt_base.h>
#include <world/block/tile_entity/tile_entity.h>

#include <stdint.h>

#define CHUNK_SIZE_WIDTH 16
#define CHUNK_SIZE_HEIGHT 128

struct world_s;

extern uint8_t chunk_is_lit;

typedef struct entity_index_pair_s {
    tile_entity_t *tile_entity;
    uint64_t index;
} entity_index_pair_t;

typedef struct chunk_s {
    uint8_t blocks[CHUNK_SIZE_WIDTH * CHUNK_SIZE_HEIGHT * CHUNK_SIZE_WIDTH];
    struct world_s *world;
    uint8_t *data; // nibblearray  block metadata
    uint8_t *sky_light_map; // nibblearray
    uint8_t *block_light_map; // nibblearray
    int8_t height_map[CHUNK_SIZE_WIDTH * CHUNK_SIZE_WIDTH];
    uint8_t lowest_block_height;
    int x_pos;
    int z_pos;
    uint64_t *entities[CHUNK_SIZE_HEIGHT / CHUNK_SIZE_WIDTH]; // list (uint64_t = pointer)
    entity_index_pair_t **tile_entity_map; // array_list of tile_entity_index_pair_t
    uint8_t is_terrain_populated;
    uint8_t is_modified;
    uint8_t has_entities; 
} chunk_t;

void chunk_create(chunk_t *chunk, struct world_s *world, int x, int z);
void chunk_create_from(chunk_t *chunk, struct world_s *world, uint8_t *data, int x, int z);
int chunk_get_height_value(chunk_t *chunk, int x, int z);
void chunk_generate_height_map(chunk_t *chunk);
void chunk_update_skylight(chunk_t *chunk, int x, int z);
void chunk_check_skylight_neighbor_height(chunk_t *chunk, int x, int z, int height);
void chunk_relight_block(chunk_t *chunk, int x, int y, int z);
uint8_t chunk_get_block_id(chunk_t *chunk, int x, int y, int z);
uint8_t chunk_set_block(chunk_t *chunk, int x, int y, int z, int block_id);
uint8_t chunk_get_block_metadata(chunk_t *chunk, int x, int y, int z);
void chunk_set_block_metadata(chunk_t *chunk, int x, int y, int z, int data);
uint8_t chunk_get_saved_light_value(chunk_t *chunk, uint8_t light_type, int x, int y, int z);
void chunk_set_light_value(chunk_t *chunk, uint8_t light_type, int x, int y, int z, uint8_t level);
uint8_t chunk_get_block_light_value(chunk_t *chunk, int x, int y, int z, uint8_t time_factor);
void chunk_write_nbt_data(chunk_t *chunk, nbt_base_t *nbt);
chunk_t chunk_read_nbt_data(struct world_s *world, nbt_base_t *nbt);
void chunk_add_entity(chunk_t *chunk, entity_t *entity);
void chunk_remove_entity_index(chunk_t *chunk, entity_t *entity, int index);
uint8_t chunk_can_block_see_sky(chunk_t *chunk, int x, int y, int z);
tile_entity_t *chunk_get_tile_entity(chunk_t *chunk, int x, int y, int z);
void chunk_set_tile_entity(chunk_t *chunk, int x, int y, int z, tile_entity_t *tile_entity);
void chunk_remove_tile_entity(chunk_t *chunk, int x, int y, int z);
void chunk_load_entities(chunk_t *chunk);
void chunk_unload_entities(chunk_t *chunk);
void chunk_get_entities(chunk_t *chunk, entity_t *entity, AABB_t box, entity_t **entity_list);
uint8_t chunk_needs_saving(chunk_t *chunk, uint8_t check_entities);