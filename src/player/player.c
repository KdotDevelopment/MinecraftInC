#include <player/player.h>

#include <gui/container/screen_crafting.h>
#include <gui/container/screen_furnace.h>
#include <item/item.h>
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
    player->inventory = inventory_player_create(entity);
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

uint8_t player_can_harvest_block(entity_t *player, block_t *block) {
    if(block->material != &materials.rock && block->material != &materials.metal) {
        return 1;
    }else {
        item_stack_t item = inventory_player_get_selected(&player->mob->player->inventory);
        return item.item_id != 0 ? item_list[item.item_id].can_harvest_block(&item_list[item.item_id], block->id) : 0;
    }
}

void player_render_crafting_screen(entity_t *player) {
    screen_t *screen = malloc(sizeof(screen_t));
    screen_crafting_create(screen, &player->mob->player->inventory);
    minecraft_set_current_screen(player->world->minecraft, screen);
}

void player_render_furnace_screen(entity_t *player, tile_entity_t *tile_entity) {
    screen_t *screen = malloc(sizeof(screen_t));
    screen_furnace_create(screen, &player->mob->player->inventory, tile_entity);
    minecraft_set_current_screen(player->world->minecraft, screen);
}