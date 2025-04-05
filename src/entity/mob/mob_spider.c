#include <entity/mob/mob_spider.h>
#include <entity/mob/mob_quadruped.h>
#include <entity/ai/ai_basic_jump_attack.h>
#include <model/model_spider.h>
#include <level/level.h>
#include <minecraft.h>

void mob_spider_create(mob_t *mob, struct level_s *level, float x, float y, float z) {
    mob_quadruped_create(mob, level, x, y, z);
    mob->type = ENTITY_MOB_SPIDER;
    mob->model_type = MODEL_SPIDER;
    mob->height_offset = 0.72;
    mob->texture_name = "mob/spider.png";
    mob->bb_width = 1.4;
    mob->bb_height = 0.9;
    entity_set_pos(&mob->entity, x, y, z);
    mob->death_score = 105;
    mob->bob_strength = 0;
    mob->ai = ai_basic_jump_attack_create(level, mob);
    mob->model = models_get(&level->minecraft->models, mob->model_type);
}