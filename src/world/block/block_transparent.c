#include <world/block/block_transparent.h>

#include <world/world.h>

block_t block_transparent_create(uint8_t id, int texture_id, block_sound_t *sound, float hardness, float resistance, material_t *material, uint8_t render_sides) {
    block_t block = block_create(id, texture_id, sound, hardness, resistance, material);

    block.transparent_render_sides = render_sides;
    block.is_opaque = 0;
    block.light_opacity = 0;

    block.can_render_side = block_transparent_can_render_side;

    block_list[id] = block;

    return block;
}

uint8_t block_transparent_can_render_side(block_t *block, struct world_s *world, int x, int y, int z, uint8_t side) {
    uint8_t block_id = world_get_block(world, x, y, z);
    return !block->transparent_render_sides && block_id == block->id ? 0 : block_can_render_side(block, world, x, y, z, side);
}