#include <entity/mob/mob_zombie.h>
#include <entity/mob/mob_humanoid.h>
#include <entity/ai/ai_basic_attack.h>
#include <model/model_zombie.h>
#include <world/world.h>
#include <minecraft.h>

void mob_zombie_create(entity_t *entity, struct world_s *world, float x, float y, float z) {
    mob_humanoid_create(entity, world, x, y, z);
    mob_t *mob = entity->mob;
    mob->entity->type = ENTITY_MOB_ZOMBIE;
    mob->model_type = MODEL_ZOMBIE;
    mob->entity->height_offset = 1.62;
    mob->texture_name = "mob/zombie.png";
    entity_set_pos(mob->entity, x, y, z);
    mob->death_score = 80;
    mob->ai = ai_basic_attack_create(world, mob);
    mob->ai.run_speed = 1;
    mob->ai.default_look_angle = 30;
    mob->entity->model = models_get(&world->minecraft->models, mob->model_type);
}