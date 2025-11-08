#include <entity/mob/mob_pig.h>
#include <entity/mob/mob_quadruped.h>
#include <model/model_pig.h>
#include <world/world.h>
#include <minecraft.h>

#include <stddef.h>

void mob_pig_create(entity_t *entity, struct world_s *world, float x, float y, float z) {
    mob_quadruped_create(entity, world, x, y, z);
    mob_t *mob = entity->mob;
    mob->entity->type = ENTITY_MOB_PIG;
    mob->model_type = MODEL_PIG;
    mob->texture_name = "mob/pig.png";
    mob->entity->height_offset = 1.72;
    mob->entity->model = models_get(&world->minecraft->models, mob->model_type);
    mob->hurt_sound = SOUND_MOB_PIG;
    mob->living_sound = SOUND_MOB_PIG;
    mob->death_sound = SOUND_MOB_PIG_DEATH;

    mob->die = mob_pig_die;
}

void mob_pig_die(mob_t *mob, mob_t *causer) {
    if(causer != NULL) {
        mob->entity->award_kill_score(mob->entity, causer->entity, 10);
    }

    int drop_count = random_next_uniform(&mob->entity->world->random) + random_next_uniform(&mob->entity->world->random) + 1;
    for(int i = 0; i < drop_count; i++) {
        //drop items brown mushroom
    }

    mob_die(mob, causer);
}