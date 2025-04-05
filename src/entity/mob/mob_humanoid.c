#include <entity/mob/mob_humanoid.h>
#include <model/model_humanoid.h>
#include <level/level.h>
#include <model/models.h>
#include <renderer/level_renderer.h>
#include <minecraft.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

void mob_humanoid_create(mob_t *mob, struct level_s *level, float x, float y, float z) {
    mob_create(mob, level);
    mob->type = ENTITY_MOB_HUMANOID;
    mob->model_type = MODEL_HUMANOID;
    mob->has_armor = random_next_uniform(&level->random) < 0.2;
    mob->has_helmet = random_next_uniform(&level->random) < 0.2;

    entity_set_pos(&mob->entity, x, y, z);

    mob->render_model = mob_humanoid_render_model;
}

void mob_humanoid_render_model(struct mob_s *mob, float time, float r, float bob, float y_rot, float x_rot, float scale) {
    mob_render_model(mob, time, r, bob, y_rot, x_rot, scale);
    model_t model = mob->level->minecraft->models.humanoid;

    glEnable(GL_ALPHA_TEST);
    glEnable(GL_CULL_FACE);

    if(mob->has_hair) {
        glDisable(GL_CULL_FACE);
        model.hair.y_rot = model.head.y_rot;
        model.hair.x_rot = model.head.x_rot;
        cube_render(&model.hair, scale);
        glEnable(GL_CULL_FACE);
    }

    if(mob->has_armor || mob->has_helmet) {
        glBindTexture(GL_TEXTURE_2D, textures_load(mob->level->renderer->textures, "armor/plate.png"));
        glDisable(GL_CULL_FACE);
        model_t armor_model = mob->level->minecraft->models.humanoid_armor;
        armor_model.head.visible = mob->has_helmet;
        armor_model.body.visible = mob->has_armor;
        armor_model.arm0.visible = mob->has_armor;
        armor_model.arm1.visible = mob->has_armor;
        armor_model.leg0.visible = 0;
        armor_model.leg1.visible = 0;
        armor_model.head.y_rot = model.head.y_rot;
        armor_model.head.x_rot = model.head.x_rot;
        armor_model.arm0.y_rot = model.arm0.y_rot;
        armor_model.arm0.x_rot = model.arm0.x_rot;
        armor_model.arm1.y_rot = model.arm1.y_rot;
        armor_model.arm1.x_rot = model.arm1.x_rot;
        armor_model.leg0.x_rot = model.leg0.x_rot;
        armor_model.leg1.x_rot = model.leg1.x_rot;
        cube_render(&armor_model.head, scale);
        cube_render(&armor_model.body, scale);
        cube_render(&armor_model.arm0, scale);
        cube_render(&armor_model.arm1, scale);
        cube_render(&armor_model.leg0, scale);
        cube_render(&armor_model.leg1, scale);
        glEnable(GL_CULL_FACE);
    }
    glDisable(GL_ALPHA_TEST);
}