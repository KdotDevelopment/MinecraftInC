#pragma once

#include <entity/entity.h>
#include <entity/mob/mob.h>
#include <player/inputs.h>
#include <player/inventory.h>

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