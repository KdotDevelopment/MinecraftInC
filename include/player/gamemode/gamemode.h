#pragma once

#include <player/player.h>
#include <entity/entity.h>
#include <world/world.h>
#include <world/mob_spawner.h>

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
    float destroy_progress;
    float destroy_progress_old;
    int destroy_sound_counter;
    int destroy_delay;
    mob_spawner_t mob_spawner;
    uint8_t gamemode_type;

    void (*init_world)(struct gamemode_s *gamemode, struct world_s *world);
    void (*destroy_block)(struct gamemode_s *gamemode, int x, int y, int z);
    uint8_t (*remove_item)(struct gamemode_s *gamemode, int item);
    void (*start_destroy_block)(struct gamemode_s *gamemode, int x, int y, int z);
    void (*continue_destroy_block)(struct gamemode_s *gamemode, int x, int y, int z, uint8_t face);
    void (*stop_destroy_block)(struct gamemode_s *gamemode);
    void (*render)(struct gamemode_s *gamemode, float delta);
    uint8_t (*use_item)(struct gamemode_s *gamemode, entity_t *player, int item);
    void (*init_player)(struct gamemode_s *gamemode, entity_t *player);
    void (*tick)(struct gamemode_s *gamemode);
    void (*open_inventory)(struct gamemode_s *gamemode);
    void (*spawn_mobs)(struct gamemode_s *gamemode, world_t *world);
    void (*adjust_player)(struct gamemode_s *gamemode, entity_t *player);
} gamemode_t;

gamemode_t gamemode_create(struct minecraft_s *minecraft);
void gamemode_init_world(struct gamemode_s *gamemode, struct world_s *world);
void gamemode_destroy_block(struct gamemode_s *gamemode, int x, int y, int z);
void gamemode_set_partial_time(struct gamemode_s *gamemode, float partial_tick);
uint8_t gamemode_remove_item(struct gamemode_s *gamemode, int item);
void gamemode_start_destroy_block(struct gamemode_s *gamemode, int x, int y, int z);
void gamemode_continue_destroy_block(struct gamemode_s *gamemode, int x, int y, int z, uint8_t face);
void gamemode_stop_destroy_block(struct gamemode_s *gamemode);
void gamemode_render(struct gamemode_s *gamemode, float delta);
uint8_t gamemode_use_item(struct gamemode_s *gamemode, entity_t *player, int item);
void gamemode_init_player(struct gamemode_s *gamemode, entity_t *player);
void gamemode_tick(struct gamemode_s *gamemode);
void gamemode_open_inventory(struct gamemode_s *gamemode);
void gamemode_spawn_mobs(struct gamemode_s *gamemode, world_t *world);
void gamemode_adjust_player(struct gamemode_s *gamemode, entity_t *player);