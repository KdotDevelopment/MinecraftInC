#pragma once

#include <item/item_stack.h>
#include <world/world.h>

#include <stdint.h>

struct item_stack_s;
struct block_s;

typedef struct item_s {
    int16_t item_id; // shifted by 256 for non-block items
    int16_t texture_id;
    uint8_t max_stack_size;
    int16_t durability;
    int16_t weapon_damage;
    int16_t icon_index;
    
    union {
        struct {
            uint8_t tool_type;
            uint8_t tool_piece;
            uint8_t proper_tool_efficiency;
            uint8_t entity_damage;
        };
        struct {
            uint8_t armor_type;
            uint8_t armor_piece;
            uint8_t damage_reduce;
        };
        struct { // item_block AND item_seeds
            uint16_t block_id;
        };
        struct {
            uint8_t heal_amount;
        };
    };

    uint8_t (*on_use)(item_stack_t *item_stack, world_t *world, int x, int y, int z, uint8_t side);
    item_stack_t (*on_right_click)(item_stack_t *item_stack, world_t *world, player_t *player);
    float (*get_strength_against_block)(struct item_s *item, struct block_s *block);
    void (*hit_entity)(struct item_s *item, item_stack_t *item_stack);
    void (*on_block_destroy)(struct item_s *item, item_stack_t *item_stack);
    int (*get_damage_against_entity)(struct item_s *item);
} item_t;

extern item_t item_list[1024];

item_t item_create(int16_t id, uint8_t texture_id);
uint8_t item_on_use(item_stack_t *item_stack, world_t *world, int x, int y, int z, uint8_t side);
item_stack_t item_on_right_click(item_stack_t *item_stack, world_t *world, player_t *player);
float item_get_strength_against_block(item_t *item, block_t *block);
void item_hit_entity(item_t *item, item_stack_t *item_stack);
void item_on_block_destroy(item_t *item, item_stack_t *item_stack);
int item_get_damage_against_entity(item_t *item);