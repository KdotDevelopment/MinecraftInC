#pragma once

#include <entity/entity.h>
#include <world/world.h>

void entity_painting_create(entity_t *entity, world_t *world, int x, int y, int z, int8_t side);
void entity_painting_set_direction(entity_t *entity, int8_t side);
void entity_painting_tick(entity_t *entity);
uint8_t entity_painting_on_valid_surface(entity_t *entity);
void entity_painting_hurt(entity_t *entity, entity_t *other, int damage);
void entity_painting_write_nbt(entity_t *entity, nbt_base_t *nbt);
void entity_painting_read_nbt(entity_t *entity, nbt_base_t *nbt);
