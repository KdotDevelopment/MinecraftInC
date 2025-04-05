#include <player/gamemode/gamemode_creative.h>
#include <gui/screen_block_select.h>
#include <session_data.h>
#include <minecraft.h>

#include <stdlib.h>

gamemode_t gamemode_creative_create(struct minecraft_s *minecraft) {
    gamemode_t gamemode = gamemode_create(minecraft);
    gamemode.instant_break = 1;
    gamemode.gamemode_type = GAMEMODE_CREATIVE;

    gamemode.open_inventory = gamemode_creative_open_inventory;
    gamemode.init_level = gamemode_creative_init_level;
    gamemode.adjust_player = gamemode_creative_adjust_player;

    return gamemode;
}

void gamemode_creative_open_inventory(struct gamemode_s *gamemode) {
    screen_t *build_screen = malloc(sizeof(screen_t));
    *build_screen = screen_block_select_create();
    minecraft_set_current_screen(gamemode->minecraft, build_screen);
}

void gamemode_creative_init_level(struct gamemode_s *gamemode, struct level_s *level) {
    gamemode_init_level(gamemode, level);
    level_remove_all_non_creative_entities(level);
    level->creative_mode = 1;
}

void gamemode_creative_adjust_player(struct gamemode_s *gamemode, player_t *player) {
    for(int i = 0; i < 9; i++) {
        player->inventory.counts[i] = 1;
        player->inventory.slots[i] = session_allowed_blocks[i];
    }
}