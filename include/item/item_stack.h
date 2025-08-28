#pragma once

#include <world/block/block.h>

#include <stdint.h>

typedef struct item_stack_s {
    uint8_t stack_size;
    int16_t animations_to_go;
    int16_t item_id;
    int16_t item_damage;
} item_stack_t;

item_stack_t item_stack_create(uint8_t block_id, uint8_t stack_size, int16_t item_damage);
item_stack_t item_stack_from_nbt(nbt_base_t *nbt);
item_stack_t item_stack_split(item_stack_t *item_stack, uint8_t split_size);
int16_t item_stack_get_max_damage(item_stack_t *item_stack);
void item_stack_damage(item_stack_t *item_stack, int16_t damage);