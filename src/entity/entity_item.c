#include <entity/entity_item.h>
#include <entity/entity_take_anim.h>
#include <level/block/blocks.h>
#include <model/model_item.h>
#include <level/level.h>

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

void entity_item_create(entity_t *entity, struct level_s *level, float x, float y, float z, int block_id) {
    entity_create(entity, level);
    entity->bb_width = 0.25;
    entity->bb_height = 0.25;
    entity->height_offset = entity->bb_height / 2.0;
    entity_set_pos(entity, x, y, z);
    entity->block_id = block_id;
    entity->y_rot = (float)(random_uniform() * 360.0);
    entity->xd = (float)(random_uniform() * 0.2 - 0.1);
    entity->yd = 0.2;
    entity->zd = (float)(random_uniform() * 0.2 - 0.1);
    entity->make_step_sound = 0;
    entity->age = 0;
    entity->tick_count = 0;

    entity->tick = entity_item_tick;
    entity->render = entity_item_render;
    entity->player_touch = entity_item_player_touch;
}

void entity_item_tick(struct entity_s *entity) {
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

void entity_item_render(struct entity_s *entity, textures_t *textures, float delta) {
    glBindTexture(GL_TEXTURE_2D, textures_load(textures, "terrain.png"));
    float brightness = level_get_brightness((level_t *)entity->level, entity->x, entity->y, entity->z);
    float rot_delta = entity->y_rot + (entity->tick_count + delta) * 3.0;
    glPushMatrix();
    glColor4f(brightness, brightness, brightness, 1.0);
    float rotation = tsin(rot_delta / 10.0);
    float hover_height = rotation * 0.1 + 0.1;
    glTranslatef(entity->xo + (entity->x - entity->xo) * delta, 
                 entity->yo + (entity->y - entity->yo) * delta + hover_height, 
                 entity->zo + (entity->z - entity->zo) * delta);
    glRotatef(rot_delta, 0, 1, 0);
    model_item_render(&item_models[entity->block_id]);

    float a = rotation * 0.5 + 0.5;
    float b = a * a;
    float alpha = b * b;
    //in the original source, this is set to (1.0, 1.0, 1.0, alpha * 0.4) however only changing alpha doesn't work in my case
    glColor4f(alpha * 0.4, alpha * 0.4, alpha * 0.4, alpha * 0.4);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_ALPHA_TEST);

    model_item_render(&item_models[entity->block_id]);
    glEnable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1, 1, 1, 1);
    glPopMatrix();
    glEnable(GL_TEXTURE_2D);
}

void entity_item_player_touch(entity_t *entity, entity_t *player) {
    player_t *real_player = (player_t *)player;
    if(inventory_add_item(&real_player->inventory, entity->block_id)) {
        entity_t *anim = malloc(sizeof(entity_t));
        entity_take_anim_create(anim, entity->level, entity, real_player);
        level_add_entity(entity->level, anim);
        entity_remove(entity);
    }
}