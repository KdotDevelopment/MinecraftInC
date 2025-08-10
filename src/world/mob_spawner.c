#include <level/mob_spawner.h>
#include <level/level.h>
#include <level/block/blocks.h>
#include <entity/mob/mob_creeper.h>
#include <entity/mob/mob_pig.h>
#include <entity/mob/mob_skeleton.h>
#include <entity/mob/mob_zombie.h>
#include <entity/mob/mob_spider.h>
#include <entity/mob/mob_sheep.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

mob_spawner_t mob_spawner_create(struct level_s *level) {
    mob_spawner_t spawner = { 0 };

    spawner.level = level;

    return spawner;
}

void mob_spawner_tick(mob_spawner_t *spawner, int level_size, entity_t *entity, progress_bar_t *progress_bar) {
    int count = 0;

    for(int i = 0; i < level_size; i++) {
        if(progress_bar != NULL) {
            progress_bar_set_progress(progress_bar, i * 100 / (level_size - 1));
        }

        int rnd = random_next_int_range(&spawner->level->random, 0, 5);
        int rndx = random_next_int_range(&spawner->level->random, 0, spawner->level->width);
        int rndy = fmin(random_next_uniform(&spawner->level->random), random_next_uniform(&spawner->level->random)) * spawner->level->depth;
        int rndz = random_next_int_range(&spawner->level->random, 0, spawner->level->height);

        if(!level_is_solid_block(spawner->level, rndx, rndy, rndz) 
          && level_get_block(spawner->level, rndx, rndy, rndz) == blocks.air.id 
          && (!level_is_lit(spawner->level, rndx, rndy, rndz) || random_next_int_range(&spawner->level->random, 0, 5) == 0)) {
            int rndx1 = rndx;
            int rndy1 = rndy;
            int rndz1 = rndz;

            for(int j = 0; j < 3; j++) {
                for(int k = 0; k < 3; k++) {
                    rndx1 += random_next_int_range(&spawner->level->random, 0, 6) - random_next_int_range(&spawner->level->random, 0, 6);
                    rndy1 += random_next_int_range(&spawner->level->random, 0, 1) - random_next_int_range(&spawner->level->random, 0, 1);
                    rndz1 += random_next_int_range(&spawner->level->random, 0, 6) - random_next_int_range(&spawner->level->random, 0, 6);

                    if(rndx1 >= 0 && rndz1 >= 1 && rndy1 >= 0 && rndy1 < spawner->level->depth - 2 && rndx1 < spawner->level->width && rndz1 < spawner->level->height
                      && level_is_solid_block(spawner->level, rndx1, rndy1 - 1, rndz1) && !level_is_solid_block(spawner->level, rndx1, rndy1, rndz1) 
                      && !level_is_solid_block(spawner->level, rndx1, rndy1 + 1, rndz1)) {
                        float rndx2 = rndx1 + 0.5;
                        float rndy2 = rndy1 + 1;
                        float rndz2 = rndz1 + 0.5;
                        if(entity != NULL) {
                            float rndx3 = rndx2 - entity->x;
                            float rndy3 = rndy2 - entity->y;
                            float rndz3 = rndz2 - entity->z;
                            float dist = (rndx3 * rndx3 + rndy3 * rndy3 + rndz3 * rndz3);
                            if(dist < 256) continue;
                        }else {
                            float rndx3 = rndx2 - spawner->level->spawn_x;
                            float rndy3 = rndy2 - spawner->level->spawn_y;
                            float rndz3 = rndz2 - spawner->level->spawn_z;
                            float dist = (rndx3 * rndx3 + rndy3 * rndy3 + rndz3 * rndz3);
                            if(dist < 256) continue;
                        }
                        mob_t *mob = malloc(sizeof(mob_t));
                        memset(mob, 0, sizeof(mob_t));

                        if(rnd == 0) mob_zombie_create(mob, spawner->level, rndx2, rndy2, rndz2);
                        else if(rnd == 1) mob_skeleton_create(mob, spawner->level, rndx2, rndy2, rndz2);
                        else if(rnd == 2) mob_pig_create(mob, spawner->level, rndx2, rndy2, rndz2);
                        else if(rnd == 3) mob_creeper_create(mob, spawner->level, rndx2, rndy2, rndz2);
                        else if(rnd == 4) mob_spider_create(mob, spawner->level, rndx2, rndy2, rndz2);
                        else if(rnd == 5) mob_sheep_create(mob, spawner->level, rndx2, rndy2, rndz2);
                        else mob_pig_create(mob, spawner->level, rndx2, rndy2, rndz2);

                        if(level_is_free(spawner->level, mob->bb)) {
                            level_add_entity(spawner->level, &mob->entity);
                            count++;
                        }else {
                            free(mob);
                        }
                    }
                }
            }
        }
    }
}