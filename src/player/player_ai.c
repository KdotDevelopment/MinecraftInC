#include <player/player_ai.h>
#include <player/player.h>
#include <entity/ai/ai_basic.h>
#include <level/level.h>

ai_t player_ai_create(struct player_s *player) {
    player_t *real_player = (player_t *)player;
    ai_t ai = ai_basic_create(real_player->level, &real_player->mob);
    ai.update = player_ai_update;
    ai.tick = player_ai_tick;
    ai.player = (struct player_s *)player;

    return ai;
}

void player_ai_tick(struct ai_s *proto_ai) {
    ai_t *ai = (ai_t *)proto_ai;
    player_t *player = (player_t *)ai->player;
    //inventory tick
    inputs_update_movement(&player->inputs);
    ai_basic_tick(proto_ai);

    float bob = sqrtf(player->xd * player->xd + player->zd * player->zd);
    float tilt = atan(-player->yd * 0.2) * 15.0;
    player->obob = player->bob;
    if(bob > 0.1) {
        bob = 0.1;
    }

    if(!entity_on_ground(&player->entity)) {
        bob = 0;
    }else {
        tilt = 0;
    }
    player->bob += (bob - player->bob) * 0.4;
    player->tilt += (tilt - player->tilt) * 0.8;
}

void player_ai_update(struct ai_s *proto_ai) {
    ai_t *ai = (ai_t *)proto_ai;
    player_t *player = (player_t *)ai->player;
    ai->jumping = player->inputs.jumping;
    ai->xa = player->inputs.x;
    ai->za = player->inputs.z;
}