#pragma once

#include <item/item_stack.h>
#include <world/world.h>

#include <stdint.h>

struct item_stack_s;

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
        struct {
            uint16_t block_id;
        };
    };

    void (*on_use)(item_stack_t *item_stack, world_t *world, int x, int y, int z, uint8_t side);
    item_stack_t (*on_right_click)(item_stack_t *item_stack, world_t *world, player_t *player);
} item_t;

extern item_t item_list[1024];

item_t item_create(int16_t id, uint8_t texture_id);
uint8_t item_on_use(item_stack_t *item_stack, world_t *world, int x, int y, int z, uint8_t side);
item_stack_t item_on_right_click(item_stack_t *item_stack, world_t *world, player_t *player);