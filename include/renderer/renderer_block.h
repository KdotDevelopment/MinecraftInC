#pragma once

#include <stdint.h>

struct world_s;
struct block_s;

typedef struct renderer_block_s {
    struct world_s *world;
    int16_t fixed_texture_id;
    uint8_t flip_texture;
} renderer_block_t;

renderer_block_t renderer_block_create(struct world_s *world);
void renderer_block_render_using_texture(renderer_block_t *renderer, struct block_s *block, int x, int y, int z, int override_texture);
void renderer_block_render_inside(renderer_block_t *renderer, struct block_s *block, int x, int y, int z);
uint8_t renderer_block_render(renderer_block_t *renderer, struct block_s *block, int x, int y, int z);
void renderer_block_render_top(renderer_block_t *renderer, struct block_s *block, double x, double y, double z, int texture_id);
void renderer_block_render_bottom(renderer_block_t *renderer, struct block_s *block, double x, double y, double z, int texture_id);
void renderer_block_render_north(renderer_block_t *renderer, struct block_s *block, double x, double y, double z, int texture_id);
void renderer_block_render_south(renderer_block_t *renderer, struct block_s *block, double x, double y, double z, int texture_id);
void renderer_block_render_west(renderer_block_t *renderer, struct block_s *block, double x, double y, double z, int texture_id);
void renderer_block_render_east(renderer_block_t *renderer, struct block_s *block, double x, double y, double z, int texture_id);
void renderer_block_render_in_inventory(renderer_block_t *renderer, struct block_s *block);