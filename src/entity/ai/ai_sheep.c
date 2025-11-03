#include <entity/ai/ai_sheep.h>
#include <entity/mob/mob.h>
#include <world/block/blocks.h>
#include <world/world.h>

ai_t ai_sheep_create(struct mob_s *mob) {
    ai_t ai = ai_basic_create(mob->entity->world, mob);
    ai.update = ai_sheep_update;
    
    return ai;
}

void ai_sheep_update(ai_t *ai) {
    float s = tsin(ai->mob->entity->y_rot * M_PI / 180.0);
    float c = tcos(ai->mob->entity->y_rot * M_PI / 180.0);
    s *= -0.7;
    c *= 0.7;
    int x = ai->mob->entity->x + s;
    int y = ai->mob->entity->y - 2.0;
    int z = ai->mob->entity->z + c;
    if(ai->mob->is_grazing) {
        if(world_get_block(ai->world, x, y, z) != blocks.grass.id) {
            ai->mob->is_grazing = 0;
        }else {
            if(++ai->mob->grazing_time == 60) {
                world_set_block_with_update(ai->world, x, y, z, blocks.dirt.id);
                if(random_next_int_range(&ai->world->random, 0, 5) == 0) {
                    ai->mob->has_fur = 1;
                }
            }

            ai->xa = 0;
            ai->za = 0;
            ai->mob->entity->x_rot = 40.0 + ((ai->mob->grazing_time / 2) % 2) * 10.0;
        }
    }else {
        if(world_get_block(ai->world, x, y, z) == blocks.grass.id) {
            ai->mob->is_grazing = 1;
            ai->mob->grazing_time = 0;
        }

        ai_basic_update(ai);
    }
}