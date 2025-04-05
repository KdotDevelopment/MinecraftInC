#include <entity/mob/mob_sheep.h>
#include <entity/mob/mob_quadruped.h>
#include <entity/ai/ai_sheep.h>
#include <level/level.h>
#include <model/models.h>
#include <renderer/level_renderer.h>
#include <model/model_sheep.h>
#include <model/model_sheep_fur.h>
#include <minecraft.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

void mob_sheep_create(mob_t *mob, struct level_s *level, float x, float y, float z) {
    mob_quadruped_create(mob, level, x, y, z);
    mob->type = ENTITY_MOB_SHEEP;
    mob->model_type = MODEL_SHEEP;
    mob->height_offset = 1.72;
    mob->texture_name = "mob/sheep.png";
    mob->has_fur = 1;
    mob->is_grazing = 0;
    mob->graze = 0.0F;
    mob->graze_o = 0.0F;
    mob->ai = ai_sheep_create(mob);
    mob->model = models_get(&level->minecraft->models, mob->model_type);

    entity_set_pos(&mob->entity, x, y, z);

    mob->die = mob_sheep_die;
    mob->hurt = mob_sheep_hurt;
    mob->render_model = mob_sheep_render_model;
    mob->ai_step = mob_sheep_ai_step;
}

void mob_sheep_die(mob_t *mob, mob_t *causer) {
    if(causer != NULL) {
        mob->award_kill_score(&mob->entity, &causer->entity, 10);
    }

    int drop_count = random_next_uniform(&mob->level->random) + random_next_uniform(&mob->level->random) + 1;

    for(int i = 0; i < drop_count; i++) {
        //drop items brown mushroom
    }

    mob_die(mob, causer);
}

void mob_sheep_ai_step(struct mob_s *mob) {
    mob_ai_step(mob);
    mob->graze_o = mob->graze;
    if(mob->is_grazing) {
        mob->graze += 0.2;
    }else {
        mob->graze -= 0.2;
    }

    if(mob->graze < 0.0) {
        mob->graze = 0.0;
    }
    if(mob->graze > 1.0) {
        mob->graze = 1.0;
    }
}

void mob_sheep_hurt(struct entity_s *mob, struct entity_s *causer, int damage) {
    mob_t *real_mob = (mob_t *)mob;
    if(causer != NULL && causer->type == ENTITY_MOB_PLAYER && real_mob->has_fur) {
        real_mob->has_fur = 0;
        int drop_count = random_next_uniform(&mob->level->random) * 3.0 + 1.0;

        for(int i = 0; i < drop_count; i++) {
            //drop items white wool
        }
    }else {
        mob_hurt(mob, causer, damage);
    }
}

void mob_sheep_render_model(struct mob_s *mob, float time, float r, float bob, float y_rot, float x_rot, float scale) {
    model_t *model = mob->model;
    float head_y = model->head.y;
    float head_z = model->head.z;
    model->head.y = model->head.y + (mob->graze_o + (mob->graze - mob->graze_o) * time) * 8.0;
    model->head.z = model->head.z - (mob->graze_o + (mob->graze - mob->graze_o) * time);
    model_quadruped_render(model, time, r, bob, y_rot, x_rot, scale);
    if(mob->has_fur) {
        glBindTexture(GL_TEXTURE_2D, textures_load(mob->level->renderer->textures, "mob/sheep_fur.png"));
        glDisable(GL_CULL_FACE);
        model_t fur_model = model_sheep_fur_create();
        fur_model.head.y_rot = model->head.y_rot;
        fur_model.head.x_rot = model->head.x_rot;
        fur_model.head.y = model->head.y;
        fur_model.head.x = model->head.x;
        fur_model.body.x_rot = model->body.x_rot;
        fur_model.body.y_rot = model->body.y_rot;
        fur_model.leg0.x_rot = model->leg0.x_rot;
        fur_model.leg1.x_rot = model->leg1.x_rot;
        fur_model.leg2.x_rot = model->leg2.x_rot;
        fur_model.leg3.x_rot = model->leg3.x_rot;
        cube_render(&fur_model.head, scale);
        cube_render(&fur_model.body, scale);
        cube_render(&fur_model.leg0, scale);
        cube_render(&fur_model.leg1, scale);
        cube_render(&fur_model.leg2, scale);
        cube_render(&fur_model.leg3, scale);
    }

    model->head.y = head_y;
    model->head.z = head_z;
}