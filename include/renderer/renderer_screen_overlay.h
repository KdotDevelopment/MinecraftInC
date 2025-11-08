#pragma once

#include <world/world.h>

typedef struct renderer_screen_overlay_s {
    world_t *world;
    item_stack_t item_stack;
    float equipped_progress;
    float equipped_progress_old;
    int swing_time;
    uint8_t is_swinging;
    renderer_block_t renderer_block;
} renderer_screen_overlay_t;

renderer_screen_overlay_t renderer_screen_overlay_create(world_t *world);
void renderer_screen_overlay_render(renderer_screen_overlay_t *renderer, float partial_tick);
void renderer_screen_overlay_render_overlays(renderer_screen_overlay_t *renderer, float partial_tick);
void renderer_screen_overlay_update_item(renderer_screen_overlay_t *renderer);