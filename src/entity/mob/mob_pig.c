#include <entity/mob/mob_pig.h>
#include <entity/mob/mob_quadruped.h>
#include <model/model_pig.h>
#include <level/level.h>
#include <minecraft.h>

#include <stddef.h>

void mob_pig_create(mob_t *mob, struct level_s *level, float x, float y, float z) {
    mob_quadruped_create(mob, level, x, y, z);
    mob->type = ENTITY_MOB_PIG;
    mob->model_type = MODEL_PIG;
    mob->texture_name = "mob/pig.png";
    mob->height_offset = 1.72;
    mob->model = models_get(&level->minecraft->models, mob->model_type);

    mob->die = mob_pig_die;
}

void mob_pig_die(mob_t *mob, mob_t *causer) {
    if(causer != NULL) {
        mob->award_kill_score(&mob->entity, &causer->entity, 10);
    }

    int drop_count = random_next_uniform(&mob->level->random) + random_next_uniform(&mob->level->random) + 1;
    for(int i = 0; i < drop_count; i++) {
        //drop items brown mushroom
    }

    mob_die(mob, causer);
}