#include <entity/mob/mob_skeleton.h>
#include <entity/mob/mob_zombie.h>
#include <model/model_skeleton.h>
#include <entity/entity.h>
#include <entity/entity_arrow.h>
#include <entity/ai/ai_skeleton.h>
#include <level/level.h>
#include <minecraft.h>

#include <stdlib.h>

void mob_skeleton_create(mob_t *mob, struct level_s *level, float x, float y, float z) {
    mob_zombie_create(mob, level, x, y, z);
    mob->type = ENTITY_MOB_SKELETON;
    mob->model_type = MODEL_SKELETON;
    mob->ai = ai_skeleton_create(level, mob);
    mob->texture_name = "mob/skeleton.png";
    mob->death_score = 120;
    mob->ai.run_speed = 0.3;
    mob->ai.damage = 8;
    mob->model = models_get(&level->minecraft->models, mob->model_type);
}

void mob_skeleton_shoot_arrow(mob_t *mob, struct level_s *level) {
    entity_t *arrow = malloc(sizeof(entity_t));
    entity_arrow_create(arrow, level, &mob->entity, mob->x, mob->y, mob->z, mob->y_rot + 180.0 + (random_uniform() * 45.0 - 22.5), mob->x_rot - (random_uniform() * 45.0 - 10), 1);
    level_add_entity(level, arrow);
}