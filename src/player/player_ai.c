#include <player/player_ai.h>
#include <player/player.h>
#include <entity/ai/ai_basic.h>
#include <world/world.h>

#include <util/array_list.h>

ai_t player_ai_create(struct player_s *player) {
    player_t *real_player = (player_t *)player;
    ai_t ai = ai_basic_create(real_player->entity->world, real_player->mob);
    ai.update = player_ai_update;
    ai.tick = player_ai_tick;
    ai.player = (struct player_s *)player;

    return ai;
}

void player_ai_tick(struct ai_s *proto_ai) {
    ai_t *ai = (ai_t *)proto_ai;
    player_t *player = (player_t *)ai->player;
    //inventory_tick(&player->inventory);
    inputs_update_movement(&player->inputs);
    ai_basic_tick(proto_ai);

    float bob = sqrtf(player->entity->xd * player->entity->xd + player->entity->zd * player->entity->zd);
    float tilt = atan(-player->entity->yd * 0.2) * 15.0;
    player->obob = player->bob;
    if(bob > 0.1) {
        bob = 0.1;
    }

    if(!entity_on_ground(player->entity)) {
        bob = 0;
    }else {
        tilt = 0;
    }
    player->bob += (bob - player->bob) * 0.4;
    player->mob->tilt += (tilt - player->mob->tilt) * 0.8;

    /*AABB_t bb = AABB_grow(player->entity.bb, 1.0, 0.0, 1.0);
    entity_t ***entities = entity_map_get_entities(&player->world->entity_map, &player->entity, bb.x0, bb.y0, bb.z0, bb.x1, bb.y1, bb.z1);
    if(player->health > 0 && entities != NULL) {
        for(int i = 0; i < array_list_length(*entities); i++) {
            entity_t *entity = *(entity_t **)array_list_get(*entities, i);
            entity->player_touch(entity, &player->entity);
        }
    }*/
}

void player_ai_update(struct ai_s *proto_ai) {
    ai_t *ai = (ai_t *)proto_ai;
    player_t *player = (player_t *)ai->player;
    ai->jumping = player->inputs.jumping;
    ai->xa = player->inputs.x;
    ai->za = player->inputs.z;
}