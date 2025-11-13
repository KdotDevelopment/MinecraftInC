#include <entity/entity_primed_tnt.h>

void entity_primed_tnt_create(entity_t *entity, world_t *world, float x, float y, float z) {
    entity_create(entity, world);
    entity->type = ENTITY_PRIMED_TNT;
}