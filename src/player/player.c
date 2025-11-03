#include <player/player.h>
#include <model/model.h>
#include <world/world.h>
#include <player/player_ai.h>
#include <minecraft.h>

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

// Extends mob/mob.h
void player_create(entity_t *entity, struct world_s *world) {
    world_t *real_world = (world_t *)world;
    mob_create(entity, world);
    entity->mob->player = malloc(sizeof(player_t));
    player_t *player = entity->mob->player;
    player->mob = entity->mob;
    player->entity = entity;

    player->bob = 0;
    player->obob = 0;

    player->entity->type = ENTITY_MOB_PLAYER;

    player->entity->height_offset = 1.62;
    player->entity->foot_size = 0.5;
    player->mob->health = 20;
    player->mob->model_type = MODEL_HUMANOID;
    player->mob->rot_offs = 100;
    player->entity->texture_id = 123;
    player->entity->bb_width = 0.6;
    player->entity->bb_height = 1.8;
    player->inventory = inventory_create();
    player->mob->ai = player_ai_create((struct player_s *)player);
    player->entity->allowed_in_creative_mode = 1;
    player->arrows = 20;
    player->entity->model = models_get(&world->minecraft->models, player->mob->model_type);

    entity_reset_pos(player->entity);

    if(world) {
        real_world->player = player->mob->entity;
        world_spawn_entity(real_world, player->entity);
    }

    //return player;
}

void player_set_key(player_t *player, uint8_t key, uint8_t state) {
    inputs_set_key_state(&player->inputs, key, state);
}