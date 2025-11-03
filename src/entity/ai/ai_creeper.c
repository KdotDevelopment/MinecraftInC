#include <entity/ai/ai_creeper.h>
#include <entity/mob/mob.h>
#include <particle/particles.h>
#include <particle/particle_terrain.h>
#include <world/world.h>
#include <world/block/blocks.h>

#include <stdlib.h>

ai_t ai_creeper_create(struct mob_s *mob) {
    ai_t ai = ai_basic_attack_create(mob->entity->world, mob);

    ai.attack = ai_creeper_attack;
    ai.before_removal = ai_creeper_before_removal;

    return ai;
}

uint8_t ai_creeper_attack(ai_t *ai) {
    if(!ai_basic_attack_attack(ai)) {
        return 0;
    }
    mob_hurt(ai->attack_target, ai->mob->entity, 6);
    return 1;
}

void ai_creeper_before_removal(ai_t *ai) {
    float radius = 4.0;
    world_explode(ai->mob->entity->world, ai->mob->entity, ai->mob->entity->x, ai->mob->entity->y, ai->mob->entity->z, radius);
    
    for(int i = 0; i < 500; i++) {
        float x = random_next_uniform(ai->random) * radius / 4.0;
        float y = random_next_uniform(ai->random) * radius / 4.0;
        float z = random_next_uniform(ai->random) * radius / 4.0;
        float dist = sqrtf(x * x + y * y + z * z);
        float xd = x / dist / dist;
        float yd = y / dist / dist;
        float zd = z / dist / dist;
        particle_t *particle = malloc(sizeof(particle_t));
        *particle = particle_terrain_create(ai->mob->entity->world, ai->mob->entity->x + x, ai->mob->entity->y + y, ai->mob->entity->z + z, xd, yd, zd, &block_list[blocks.leaves.id]);
        particles_spawn_particle(ai->mob->entity->world->particles, particle);
    }
}