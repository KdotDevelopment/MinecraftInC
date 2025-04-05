#include <player/gamemode/gamemode.h>
#include <level/block/blocks.h>
#include <minecraft.h>

gamemode_t gamemode_create(struct minecraft_s *minecraft) {
    gamemode_t gamemode = { 0 };
    gamemode.minecraft = minecraft;
    gamemode.instant_break = 0;
    gamemode.is_survival = 1;
    gamemode.reach = 5;

    gamemode.destroy_block = gamemode_destroy_block;
    gamemode.remove_item = gamemode_remove_item;
    gamemode.start_destroy_block = gamemode_start_destroy_block;
    gamemode.continue_destroy_block = gamemode_continue_destroy_block;
    gamemode.stop_destroy_block = gamemode_stop_destroy_block;
    gamemode.render = gamemode_render;
    gamemode.use_item = gamemode_use_item;
    gamemode.init_player = gamemode_init_player;
    gamemode.tick = gamemode_tick;
    gamemode.open_inventory = gamemode_open_inventory;
    gamemode.spawn_mobs = gamemode_spawn_mobs;
    gamemode.adjust_player = gamemode_adjust_player;

    return gamemode;
}

void gamemode_init_level(struct gamemode_s *gamemode, struct level_s *level) {
    level->creative_mode = 0;
}

void gamemode_destroy_block(struct gamemode_s *gamemode, int x, int y, int z) {
    level_t *level = (level_t *)&gamemode->minecraft->level;
    block_t *block = &block_list[level_get_block(level, x, y, z)];
    if(block != NULL) {
        if(block->sound.type != BLOCK_SOUND_NONE) {
            level_play_sound_at(level, block->sound.name, x, y, z, block->sound.volume, block->sound.pitch);
        }
        block->destroy(block, level, x, y, z, &gamemode->minecraft->particles);
    }
    level_set_block(level, x, y, z, blocks.air.id);
}

uint8_t gamemode_remove_item(struct gamemode_s *gamemode, int item) {
    return 1;
}

void gamemode_start_destroy_block(struct gamemode_s *gamemode, int x, int y, int z) {
    gamemode_destroy_block(gamemode, x, y, z);
}

void gamemode_continue_destroy_block(struct gamemode_s *gamemode, int x, int y, int z, int face) {
    return;
}

void gamemode_stop_destroy_block(struct gamemode_s *gamemode) {
    return;
}

void gamemode_render(struct gamemode_s *gamemode, float delta) {
    return;
}

uint8_t gamemode_use_item(struct gamemode_s *gamemode, player_t *player, int item) {
    return 0;
}

void gamemode_init_player(struct gamemode_s *gamemode, player_t *player) {
    return;
}

void gamemode_tick(struct gamemode_s *gamemode) {
    return;
}

void gamemode_open_inventory(struct gamemode_s *gamemode) {
    return;
}

void gamemode_spawn_mobs(struct gamemode_s *gamemode, level_t *level) {
    return;
}

void gamemode_adjust_player(struct gamemode_s *gamemode, player_t *player) {
    return;
}