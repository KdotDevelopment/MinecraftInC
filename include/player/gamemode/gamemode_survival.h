#pragma once

#include <player/gamemode/gamemode.h>

gamemode_t gamemode_survival_create(struct minecraft_s *minecraft);
void gamemode_survival_init_player(struct gamemode_s *gamemode, player_t *player);
void gamemode_survival_destroy_block(struct gamemode_s *gamemode, int x, int y, int z);
uint8_t gamemode_survival_remove_item(struct gamemode_s *gamemode, int item);
void gamemode_survival_start_destroy_block(struct gamemode_s *gamemode, int x, int y, int z);
void gamemode_survival_continue_destroy_block(struct gamemode_s *gamemode, int x, int y, int z, int face);
void gamemode_survival_stop_destroy_block(struct gamemode_s *gamemode);
void gamemode_survival_render(struct gamemode_s *gamemode, float delta);
uint8_t gamemode_survival_use_item(struct gamemode_s *gamemode, player_t *player, int item);
void gamemode_survival_tick(struct gamemode_s *gamemode);
void gamemode_survival_init_level(struct gamemode_s *gamemode, struct level_s *level);