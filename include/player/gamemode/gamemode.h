#pragma once

#include <player/player.h>
#include <level/level.h>
#include <level/mob_spawner.h>

#include <stdint.h>

struct minecraft_s;

enum {
    GAMEMODE_CREATIVE,
    GAMEMODE_SURVIVAL,
};

typedef struct gamemode_s {
    struct minecraft_s *minecraft;
    uint8_t instant_break;
    uint8_t is_survival;
    int reach;
    int x_destroy_block;
    int y_destroy_block;
    int z_destroy_block;
    int destroy_progress;
    int destroy_progress_old;
    int destroy_delay;
    mob_spawner_t mob_spawner;
    uint8_t gamemode_type;

    void (*init_level)(struct gamemode_s *gamemode, struct level_s *level);
    void (*destroy_block)(struct gamemode_s *gamemode, int x, int y, int z);
    uint8_t (*remove_item)(struct gamemode_s *gamemode, int item);
    void (*start_destroy_block)(struct gamemode_s *gamemode, int x, int y, int z);
    void (*continue_destroy_block)(struct gamemode_s *gamemode, int x, int y, int z, int face);
    void (*stop_destroy_block)(struct gamemode_s *gamemode);
    void (*render)(struct gamemode_s *gamemode, float delta);
    uint8_t (*use_item)(struct gamemode_s *gamemode, player_t *player, int item);
    void (*init_player)(struct gamemode_s *gamemode, player_t *player);
    void (*tick)(struct gamemode_s *gamemode);
    void (*open_inventory)(struct gamemode_s *gamemode);
    void (*spawn_mobs)(struct gamemode_s *gamemode, level_t *level);
    void (*adjust_player)(struct gamemode_s *gamemode, player_t *player);
} gamemode_t;

gamemode_t gamemode_create(struct minecraft_s *minecraft);
void gamemode_init_level(struct gamemode_s *gamemode, struct level_s *level);
void gamemode_destroy_block(struct gamemode_s *gamemode, int x, int y, int z);
uint8_t gamemode_remove_item(struct gamemode_s *gamemode, int item);
void gamemode_start_destroy_block(struct gamemode_s *gamemode, int x, int y, int z);
void gamemode_continue_destroy_block(struct gamemode_s *gamemode, int x, int y, int z, int face);
void gamemode_stop_destroy_block(struct gamemode_s *gamemode);
void gamemode_render(struct gamemode_s *gamemode, float delta);
uint8_t gamemode_use_item(struct gamemode_s *gamemode, player_t *player, int item);
void gamemode_init_player(struct gamemode_s *gamemode, player_t *player);
void gamemode_tick(struct gamemode_s *gamemode);
void gamemode_open_inventory(struct gamemode_s *gamemode);
void gamemode_spawn_mobs(struct gamemode_s *gamemode, level_t *level);
void gamemode_adjust_player(struct gamemode_s *gamemode, player_t *player);