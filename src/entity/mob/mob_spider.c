#include <entity/mob/mob_spider.h>
#include <entity/mob/mob_quadruped.h>
#include <entity/ai/ai_basic_jump_attack.h>
#include <model/model_spider.h>
#include <world/world.h>
#include <minecraft.h>

void mob_spider_create(entity_t *entity, struct world_s *world, float x, float y, float z) {
    mob_quadruped_create(entity, world, x, y, z);
    mob_t *mob = entity->mob;
    mob->entity->type = ENTITY_MOB_SPIDER;
    mob->model_type = MODEL_SPIDER;
    mob->entity->height_offset = 0.72;
    mob->texture_name = "mob/spider.png";
    mob->entity->bb_width = 1.4;
    mob->entity->bb_height = 0.9;
    entity_set_pos(mob->entity, x, y, z);
    mob->death_score = 105;
    mob->bob_strength = 0;
    mob->ai = ai_basic_jump_attack_create(world, mob);
    mob->entity->model = models_get(&world->minecraft->models, mob->model_type);
}