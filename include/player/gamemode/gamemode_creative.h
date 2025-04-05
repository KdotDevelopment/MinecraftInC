#pragma once

#include <player/gamemode/gamemode.h>

gamemode_t gamemode_creative_create(struct minecraft_s *minecraft);
void gamemode_creative_open_inventory(struct gamemode_s *gamemode);
void gamemode_creative_init_level(struct gamemode_s *gamemode, struct level_s *level);
void gamemode_creative_adjust_player(struct gamemode_s *gamemode, player_t *player);