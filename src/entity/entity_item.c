#include <entity/entity_item.h>

#include <sound/sounds.h>
#include <world/block/blocks.h>
#include <model/model_item.h>
#include <world/world.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include <stddef.h>

model_t item_models[256] = { 0 };

void item_models_init() {
    for(int i = 0; i < 256; i++) {
        if(block_list[i].id != 0) {
            item_models[i] = model_item_create(block_list[i].texture_id);
        }
    }
}

void entity_item_create(entity_t *entity, struct world_s *world, float x, float y, float z, item_stack_t item_stack) {
    entity_create(entity, world);
    entity->bb_width = 0.25;
    entity->bb_height = 0.25;
    entity->height_offset = entity->bb_height / 2.0;
    entity_set_pos(entity, x, y, z);
    entity->item_stack = item_stack;
    entity->y_rot = (float)(random_uniform() * 360.0);
    entity->xd = (float)(random_uniform() * 0.2 - 0.1);
    entity->yd = 0.2;
    entity->zd = (float)(random_uniform() * 0.2 - 0.1);
    entity->make_step_sound = 0;
    entity->age = 0;
    entity->tick_count = 0;
    entity->type = ENTITY_ITEM;
    entity->hover_start = random_uniform() *  M_PI * 2.0;

    entity->tick = entity_item_tick;
    entity->player_touch = entity_item_player_touch;
}

void entity_item_tick(struct entity_s *entity) {
    if(entity->delay_before_pickup > 0) {
        entity->delay_before_pickup--;
    }
    entity->xo = entity->x;
    entity->yo = entity->y;
    entity->zo = entity->z;
    entity->yd -= 0.04;
    entity_move(entity, entity->xd, entity->yd, entity->zd);
    entity->xd *= 0.98;
    entity->yd *= 0.98;
    entity->zd *= 0.98;
    if(entity->on_ground) {
        entity->xd *= 0.7;
        entity->zd *= 0.7;
        entity->yd *= -0.5;
    }

    entity->tick_count++;
    entity->age++;
    if(entity->age > 6000) {
        entity_remove(entity);
    }
}

void entity_item_player_touch(entity_t *entity, entity_t *player) {
    if(entity->delay_before_pickup == 0 && !entity->is_dead && inventory_player_add_item(&player->mob->player->inventory, &entity->item_stack)) {
        //entity_t *anim = malloc(sizeof(entity_t));
        //entity_take_anim_create(anim, entity->world, entity, real_player);
        //world_spawn_entity(entity->world, anim);
        world_play_sound_at_entity(entity->world, player, SOUND_RANDOM_POP, 0.2, ((random_uniform() - random_uniform()) * 0.7 + 1.0) * 2.0);
        entity_remove(entity);
    }
}