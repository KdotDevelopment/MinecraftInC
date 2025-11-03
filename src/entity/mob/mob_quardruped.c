#include <entity/mob/mob_quadruped.h>

void mob_quadruped_create(entity_t *entity, struct world_s *world, float x, float y, float z) {
    mob_create(entity, world);
    mob_t *mob = entity->mob;
    mob->model_type = MODEL_PIG;
    mob->entity->bb_width = 1.4;
    mob->entity->bb_height = 1.2;

    entity_set_pos(entity, x, y, z);
}