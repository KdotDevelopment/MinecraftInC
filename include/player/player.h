#pragma once

#include <entity/entity.h>
#include <entity/mob/mob.h>
#include <player/inputs.h>
#include <player/inventory_player.h>
#include <world/block/tile_entity/tile_entity.h>

struct world_s;

typedef struct player_s {
    mob_t *mob;
    entity_t *entity;
    inputs_t inputs;
    inventory_t inventory;
    float bob;
    float obob;
    int arrows;
    int score;
} player_t;

void player_create(entity_t *entity, struct world_s *world);
void player_set_key(player_t *player, uint8_t key, uint8_t state);
uint8_t player_can_harvest_block(entity_t *player, block_t *block);
void player_render_crafting_screen(entity_t *player);
void player_render_furnace_screen(entity_t *player, tile_entity_t *tile_entity);