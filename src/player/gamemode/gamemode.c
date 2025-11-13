#include <player/gamemode/gamemode.h>

#include <world/block/blocks.h>
#include <minecraft.h>

gamemode_t gamemode_create(struct minecraft_s *minecraft) {
    gamemode_t gamemode = { 0 };
    gamemode.minecraft = minecraft;
    gamemode.instant_break = 0;
    gamemode.is_survival = 1;
    gamemode.reach = 4;

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

void gamemode_init_world(struct gamemode_s *gamemode, struct world_s *world) {
    world->creative_mode = 0;
}

uint8_t gamemode_destroy_block(struct gamemode_s *gamemode, int x, int y, int z) {
    world_t *world = gamemode->minecraft->world;
    block_t *block = &block_list[world_get_block(world, x, y, z)];
    uint8_t broken = world_set_block_with_update(world, x, y, z, blocks.air.id);
    if(block != NULL) {
        if(block->sound->base_type != BLOCK_SOUND_NONE) {
            world_play_sound(world, x + 0.5, y + 0.5, z + 0.5, block->sound->base_type, (block->sound->volume + 1.0) / 2.0, block->sound->pitch * 0.8);
        }
        block->on_destroyed(block, world, x, y, z, world_get_block_metadata(world, x, y, z));
    }
    return broken;
}

// sets damage time for breaking blocks
void gamemode_set_partial_time(struct gamemode_s *gamemode, float partial_tick) {
    if(gamemode->destroy_progress <= 0) {
        gamemode->minecraft->renderer_world.destroy_progress = 0;
    }else {
        partial_tick = gamemode->destroy_progress_old + (gamemode->destroy_progress - gamemode->destroy_progress_old) * partial_tick;
        gamemode->minecraft->renderer_world.destroy_progress = partial_tick;
    }
}

uint8_t gamemode_remove_item(struct gamemode_s *gamemode, int item) {
    return 1;
}

void gamemode_start_destroy_block(struct gamemode_s *gamemode, int x, int y, int z) {
    gamemode_destroy_block(gamemode, x, y, z);
}

void gamemode_continue_destroy_block(struct gamemode_s *gamemode, int x, int y, int z, uint8_t face) {
    return;
}

void gamemode_stop_destroy_block(struct gamemode_s *gamemode) {
    return;
}

void gamemode_render(struct gamemode_s *gamemode, float delta) {
    return;
}

uint8_t gamemode_use_item(struct gamemode_s *gamemode, entity_t *player, int item) {
    return 0;
}

void gamemode_init_player(struct gamemode_s *gamemode, entity_t *player) {
    return;
}

void gamemode_tick(struct gamemode_s *gamemode) {
    return;
}

void gamemode_open_inventory(struct gamemode_s *gamemode) {
    return;
}

void gamemode_spawn_mobs(struct gamemode_s *gamemode, world_t *world) {
    return;
}

void gamemode_adjust_player(struct gamemode_s *gamemode, entity_t *player) {
    return;
}