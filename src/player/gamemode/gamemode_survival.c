#include <player/gamemode/gamemode_survival.h>
#include <level/block/blocks.h>
#include <level/level.h>
#include <minecraft.h>

gamemode_t gamemode_survival_create(struct minecraft_s *minecraft) {
    gamemode_t gamemode = gamemode_create(minecraft);
    gamemode.instant_break = 0;
    gamemode.gamemode_type = GAMEMODE_SURVIVAL;

    gamemode.init_player = gamemode_survival_init_player;
    gamemode.destroy_block = gamemode_survival_destroy_block;
    gamemode.remove_item = gamemode_survival_remove_item;
    gamemode.start_destroy_block = gamemode_survival_start_destroy_block;
    gamemode.continue_destroy_block = gamemode_survival_continue_destroy_block;
    gamemode.stop_destroy_block = gamemode_survival_stop_destroy_block;
    gamemode.render = gamemode_survival_render;
    gamemode.use_item = gamemode_survival_use_item;
    gamemode.tick = gamemode_survival_tick;
    gamemode.init_level = gamemode_survival_init_level;

    return gamemode;
}

void gamemode_survival_init_player(struct gamemode_s *gamemode, player_t *player) {
    player->inventory.slots[0] = blocks.tnt.id;
    player->inventory.counts[0] = 10;
}

void gamemode_survival_destroy_block(struct gamemode_s *gamemode, int x, int y, int z) {
    //int block_id = level_get_block(&gamemode->minecraft->level, x, y, z);
    //block spawn drops
    gamemode_destroy_block(gamemode, x, y, z);
}

uint8_t gamemode_survival_remove_item(struct gamemode_s *gamemode, int item) {
    return inventory_remove_item(&gamemode->minecraft->player.inventory, item);
}

void gamemode_survival_start_destroy_block(struct gamemode_s *gamemode, int x, int y, int z) {
    int block_id = level_get_block(&gamemode->minecraft->level, x, y, z);
    if(block_id != blocks.air.id && block_list[block_id].destroy_speed == 0) {
        gamemode_destroy_block(gamemode, x, y, z);
    }
}

void gamemode_survival_continue_destroy_block(struct gamemode_s *gamemode, int x, int y, int z, int face) {
    if(gamemode->destroy_delay > 0) {
        gamemode->destroy_delay--;
    }else if(x == gamemode->x_destroy_block && y == gamemode->y_destroy_block && z == gamemode->z_destroy_block) {
        int block_id = level_get_block(&gamemode->minecraft->level, x, y, z);
        if(block_id != blocks.air.id) {
            block_t *block = &block_list[block_id];
            gamemode->destroy_progress_old = block->destroy_speed;
            block_breaking(block, &gamemode->minecraft->level, x, y, z, face, &gamemode->minecraft->particles);
            gamemode->destroy_progress++;
            if(gamemode->destroy_progress == gamemode->destroy_progress_old + 1) {
                gamemode_destroy_block(gamemode, x, y, z);
                gamemode->destroy_progress = 0;
                gamemode->destroy_delay = 5;
            }
        }
    }else {
        gamemode->destroy_progress = 0;
        gamemode->x_destroy_block = x;
        gamemode->y_destroy_block = y;
        gamemode->z_destroy_block = z;
    }
}

void gamemode_survival_stop_destroy_block(struct gamemode_s *gamemode) {
    gamemode->destroy_progress = 0;
    gamemode->destroy_delay = 0;
}

void gamemode_survival_render(struct gamemode_s *gamemode, float delta) {
    if(gamemode->destroy_progress <= 0) {
        gamemode->minecraft->level_renderer.destroy_progress = 0;
    }else {
        gamemode->minecraft->level_renderer.destroy_progress = ((float)gamemode->destroy_progress + delta - 1.0) / (float)gamemode->destroy_progress_old;
    }
}

uint8_t gamemode_survival_use_item(struct gamemode_s *gamemode, player_t *player, int item) {
    if(item == blocks.red_mushroom.id && inventory_remove_item(&player->inventory, item)) {
        player->hurt(&player->entity, NULL, 3);
        return 1;
    }
    if(item == blocks.brown_mushroom.id && inventory_remove_item(&player->inventory, item)) {
        player->heal(&player->entity, 5);
        return 1;
    }
    return 0;
}

void gamemode_survival_tick(struct gamemode_s *gamemode) {
    int size = gamemode->mob_spawner.level->width * gamemode->mob_spawner.level->height * gamemode->mob_spawner.level->depth / 64 / 64 / 64;
    if(random_next_int_range(&gamemode->minecraft->level.random, 0, 100) < size) { //and number of mobs < size * 20
        mob_spawner_tick(&gamemode->mob_spawner, size, &gamemode->mob_spawner.level->player->entity, &gamemode->minecraft->progress_bar);
    }
}

void gamemode_survival_init_level(struct gamemode_s *gamemode, struct level_s *level) {
    gamemode_init_level(gamemode, level);
    gamemode->mob_spawner = mob_spawner_create(level);
    progress_bar_set_text(&gamemode->minecraft->progress_bar, "Spawning...");
    int size = level->width * level->height * level->depth / 800;
    mob_spawner_tick(&gamemode->mob_spawner, size, NULL, &gamemode->minecraft->progress_bar);
}