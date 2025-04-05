#pragma once

#include <entity/entity.h>
#include <gui/progress_bar.h>

struct level_s;

typedef struct mob_spawner_s {
    struct level_s *level;
} mob_spawner_t;

mob_spawner_t mob_spawner_create(struct level_s *level);
void mob_spawner_tick(mob_spawner_t *spawner, int level_size, entity_t *entity, progress_bar_t *progress_bar) ;