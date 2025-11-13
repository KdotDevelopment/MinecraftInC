#include <entity/mob/mob_giant.h>

#include <entity/mob/mob.h>

void mob_giant_create(entity_t *entity, world_t *world, float x, float y, float z) {
    mob_create(entity, world);
    entity->type = ENTITY_MOB_GIANT;
}