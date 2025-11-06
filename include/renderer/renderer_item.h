#pragma once

#include <world/world.h>

typedef struct renderer_item_s {
    world_t *world;
    item_stack_t item_stack;
    float equipped_progress;
    float equipped_progress_old;
    int selected_slot;
    uint8_t bool;
    renderer_block_t renderer_block;
} renderer_item_t;