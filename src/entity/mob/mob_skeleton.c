#include <entity/mob/mob_skeleton.h>
#include <entity/mob/mob_zombie.h>
#include <model/model_skeleton.h>
#include <entity/entity.h>
#include <entity/entity_arrow.h>
#include <entity/ai/ai_skeleton.h>
#include <world/world.h>
#include <minecraft.h>

#include <stdlib.h>

void mob_skeleton_create(entity_t *entity, struct world_s *world, float x, float y, float z) {
    mob_zombie_create(entity, world, x, y, z);
    mob_t *mob = entity->mob;
    mob->entity->type = ENTITY_MOB_SKELETON;
    mob->model_type = MODEL_SKELETON;
    mob->ai = ai_skeleton_create(world, mob);
    mob->texture_name = "mob/skeleton.png";
    mob->death_score = 120;
    mob->ai.run_speed = 0.3;
    mob->ai.damage = 8;
    mob->entity->model = models_get(&world->minecraft->models, mob->model_type);
}

void mob_skeleton_shoot_arrow(mob_t *mob, struct world_s *world) {
    entity_t *arrow = malloc(sizeof(entity_t));
    entity_arrow_create(arrow, world, mob->entity, mob->entity->x, mob->entity->y, mob->entity->z, mob->entity->y_rot + 180.0 + (random_uniform() * 45.0 - 22.5), mob->entity->x_rot - (random_uniform() * 45.0 - 10), 1);
    world_spawn_entity(world, arrow);
}