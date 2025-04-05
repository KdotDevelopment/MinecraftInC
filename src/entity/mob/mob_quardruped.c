#include <entity/mob/mob_quadruped.h>

void mob_quadruped_create(mob_t *mob, struct level_s *level, float x, float y, float z) {
    mob_create(mob, level);
    mob->model_type = MODEL_PIG;
    mob->bb_width = 1.4;
    mob->bb_height = 1.2;

    entity_set_pos(&mob->entity, x, y, z);
}