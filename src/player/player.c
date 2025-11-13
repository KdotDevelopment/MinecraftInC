#include <player/player.h>

#include <entity/mob/mob_humanoid.h>
#include <gui/container/screen_crafting.h>
#include <gui/container/screen_furnace.h>
#include <gui/container/screen_chest.h>
#include <item/item.h>
#include <model/model.h>
#include <nbt/nbt_tag_list.h>
#include <world/world.h>
#include <player/player_ai.h>
#include <minecraft.h>

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

// Extends mob/mob.h
void player_create(entity_t *entity, struct world_s *world) {
    mob_humanoid_create(entity, world, world->spawn_x, world->spawn_y, world->spawn_z);
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

    player->entity->read_nbt = player_read_nbt;
    player->entity->write_nbt = player_write_nbt;

    entity_reset_pos(player->entity);

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

void player_read_nbt(entity_t *player, nbt_base_t *nbt) {
    mob_humanoid_read_nbt(player, nbt);
    player->mob->player->score = nbt_tag_compound_get_int(nbt, "Score");
    nbt_base_t inventory_nbt = nbt_tag_compound_get_tag_list(nbt, "Inventory");
    inventory_t *inventory = &player->mob->player->inventory;

    for(int i = 0; i < array_list_length(inventory_nbt.tag_array); i++) {
        nbt_base_t *item_nbt = nbt_tag_list_get_tag(&inventory_nbt, i);
        int slot = nbt_tag_compound_get_byte(item_nbt, "Slot") & 255;
        if(slot >= 0 && slot < 36) {
            inventory->inv[i] = item_stack_from_nbt(item_nbt);
        }
        if(slot >= 100 && slot < 104) {
            inventory->armor[i - 100] = item_stack_from_nbt(item_nbt);
        }
    }

    nbt_tag_compound_free(&inventory_nbt);
}

void player_write_nbt(entity_t *player, nbt_base_t *nbt) {
    mob_humanoid_write_nbt(player, nbt);
    nbt_tag_compound_set_int(nbt, "Score", player->mob->player->score);
    inventory_t *inventory = &player->mob->player->inventory;
    nbt_base_t inventory_nbt = nbt_tag_list_create();

    for(int i = 0; i < 36; i++) {
        nbt_base_t item_nbt = nbt_tag_compound_create();
        nbt_tag_compound_set_byte(&item_nbt, "Slot", i);
        item_stack_write_nbt(&inventory->inv[i], &item_nbt);
        nbt_tag_list_set_tag(&inventory_nbt, &item_nbt);
    }

    for(int i = 0; i < 4; i++) {
        nbt_base_t item_nbt = nbt_tag_compound_create();
        nbt_tag_compound_set_byte(&item_nbt, "Slot", i + 100);
        item_stack_write_nbt(&inventory->armor[i], &item_nbt);
        nbt_tag_list_set_tag(&inventory_nbt, &item_nbt);
    }

    nbt_tag_compound_set_tag(nbt, "Inventory", &inventory_nbt);
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

void player_render_chest_screen(entity_t *player, tile_entity_t *tile_entity) {
    screen_t *screen = malloc(sizeof(screen_t));
    screen_chest_create(screen, &player->mob->player->inventory, tile_entity);
    minecraft_set_current_screen(player->world->minecraft, screen);
}