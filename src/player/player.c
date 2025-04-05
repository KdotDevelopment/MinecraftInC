#include <player/player.h>
#include <model/model.h>
#include <level/level.h>
#include <player/player_ai.h>
#include <minecraft.h>

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

// Extends mob/mob.h
void player_create(player_t *player, struct level_s *level) {
    level_t *real_level = (level_t *)level;
    mob_create(&player->mob, level);

    player->bob = 0;
    player->obob = 0;

    player->type = ENTITY_MOB_PLAYER;

    player->height_offset = 1.62;
    player->foot_size = 0.5;
    player->health = 20;
    player->model_type = MODEL_HUMANOID;
    player->rot_offs = 100;
    player->texture_id = 123;
    player->bb_width = 0.6;
    player->bb_height = 1.8;
    player->inventory = inventory_create();
    player->ai = player_ai_create((struct player_s *)player);
    player->allowed_in_creative_mode = 1;
    player->arrows = 20;
    player->model = models_get(&level->minecraft->models, player->model_type);

    entity_reset_pos(&player->entity);

    if(level) {
        real_level->player = player;
        level_add_entity(real_level, &player->entity);
    }

    //return player;
}

void player_set_key(player_t *player, uint8_t key, uint8_t state) {
    inputs_set_key_state(&player->inputs, key, state);
}