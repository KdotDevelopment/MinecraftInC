#pragma once

#include <entity/entity.h>
#include <gui/progress_bar.h>

struct world_s;

typedef struct mob_spawner_s {
    struct world_s *world;
} mob_spawner_t;

mob_spawner_t mob_spawner_create(struct world_s *world);
void mob_spawner_tick(mob_spawner_t *spawner, int world_size, entity_t *entity, progress_bar_t *progress_bar) ;