#pragma once

#include <entity/mob/mob.h>
#include <player/inputs.h>
#include <player/inventory.h>

struct level_s;

typedef struct {
    union {
        mob_t;
        mob_t mob;
    };
    inputs_t inputs;
    inventory_t inventory;
    float bob;
    float obob;
    int arrows;
    int score;
} player_t;

void player_create(player_t *player, struct level_s *level);
void player_set_key(player_t *player, uint8_t key, uint8_t state);