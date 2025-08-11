#include <entity/entity_take_anim.h>

#include <entity/entity_item.h>
#include <world/level.h>
#include <minecraft.h>
#include <player/player.h>

#include <stdlib.h>

void entity_take_anim_create(entity_t *entity, struct level_s *level, entity_t *item, player_t *player) {
    entity_create(entity, level);
    entity->bb_width = 1.0;
    entity->bb_height = 1.0;
    entity->type = ENTITY_ITEM_TAKE;

    entity->item = malloc(sizeof(entity_t));
    //entity_create(item, level);
    memset(entity->item, 0, sizeof(entity_t));
    entity->item->x = item->x;
    entity->item->y = item->y;
    entity->item->z = item->z;
    entity->item->xo = item->xo;
    entity->item->yo = item->yo;
    entity->item->zo = item->zo;
    entity->item->xd = item->xd;
    entity->item->yd = item->yd;
    entity->item->zd = item->zd;
    entity->item->x_rot = item->x_rot;
    entity->item->y_rot = item->y_rot;
    entity->item->x_roto = item->x_roto;
    entity->item->y_roto = item->y_roto;
    entity->item->block_id = item->block_id;
    entity->item->render = item->render;
    entity->item->type = ENTITY_ITEM_TAKE_MOCK;
    entity->item->level = entity->level;
    entity->item->render = item->render;

    entity->player = player;
    entity->xorg = item->x;
    entity->yorg = item->y;
    entity->zorg = item->z;
    entity->time = 0;

    entity->tick = entity_take_anim_tick;
    entity->render = entity_take_anim_render;
}

void entity_take_anim_tick(entity_t *entity) {
    entity->time++;
    if(entity->time >= 3) {
        if(entity->item != NULL) free(entity->item);
        entity->item = NULL;
        entity_remove(entity);
        //return;
    }

    float time = (float)entity->time / 3.0;
    time = time * time;
    if(entity->item == NULL) return;
    //printf("entity_take_anim_tick %f %f %f %d\n", entity->item->x, entity->item->y, entity->item->z, entity->time);
    entity->item->xo = entity->item->x;
    entity->item->yo = entity->item->y;
    entity->item->zo = entity->item->z;
    entity->xo = entity->item->xo;
    entity->yo = entity->item->yo;
    entity->zo = entity->item->zo;
    entity->item->x = entity->xorg + (entity->player->x - entity->xorg) * time;
    entity->item->y = entity->yorg + (entity->player->y - 1.0 - entity->yorg) * time;
    entity->item->z = entity->zorg + (entity->player->z - entity->zorg) * time;
    entity->x = entity->item->x;
    entity->y = entity->item->y;
    entity->z = entity->item->z;
    entity_set_pos(entity, entity->item->x, entity->item->y, entity->item->z);
}

void entity_take_anim_render(entity_t *entity, textures_t *textures, float delta) {
    if(entity->item == NULL) return;
    //printf("test\n");
    entity->item->render(entity->item, textures, delta);
}