#include <entity/mob/mob_zombie.h>
#include <entity/mob/mob_humanoid.h>
#include <entity/ai/ai_basic_attack.h>
#include <model/model_zombie.h>
#include <level/level.h>
#include <minecraft.h>

void mob_zombie_create(mob_t *mob, struct level_s *level, float x, float y, float z) {
    mob_humanoid_create(mob, level, x, y, z);
    mob->type = ENTITY_MOB_ZOMBIE;
    mob->model_type = MODEL_ZOMBIE;
    mob->height_offset = 1.62;
    mob->texture_name = "mob/zombie.png";
    entity_set_pos(&mob->entity, x, y, z);
    mob->death_score = 80;
    mob->ai = ai_basic_attack_create(level, mob);
    mob->ai.run_speed = 1;
    mob->ai.default_look_angle = 30;
    mob->model = models_get(&level->minecraft->models, mob->model_type);
}