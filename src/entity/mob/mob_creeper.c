#include <entity/mob/mob_creeper.h>
#include <entity/ai/ai_creeper.h>
#include <entity/ai/ai_skeleton.h>
#include <model/model_creeper.h>
#include <entity/entity.h>
#include <level/level.h>
#include <minecraft.h>

void mob_creeper_create(mob_t *mob, struct level_s *level, float x, float y, float z) {
    mob_create(mob, level);
    mob->type = ENTITY_MOB_CREEPER;
    mob->model_type = MODEL_CREEPER;
    mob->height_offset = 1.62;
    mob->death_score = 200;
    mob->texture_name = "mob/creeper.png";
    mob->ai = ai_creeper_create(mob);
    mob->ai.default_look_angle = 45;
    mob->model = models_get(&level->minecraft->models, mob->model_type);

    entity_set_pos(&mob->entity, x, y, z);

    mob->get_brightness = mob_creeper_get_brightness;
}

float mob_creeper_get_brightness(struct entity_s *entity, float tick) {
    float health_factor = (20 - ((mob_t *)entity)->health) / 20.0;
    return ((tsin(((mob_t *)entity)->tick_count + tick) * 0.5 + 0.5) * health_factor * 0.5 + 0.25 + health_factor * 0.25) * entity_get_brightness(entity, tick);
}