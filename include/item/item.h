#pragma once

#include <stdint.h>

typedef struct item_s {
    int item_id;
    int texture_id;
    int max_stack_size;
    int durability;
    int weapon_damage;
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
        };
    };
} item_t;

item_t item_create(uint8_t id, uint8_t texture_id);