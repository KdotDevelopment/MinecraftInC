#include <level/block/block_liquid.h>
#include <level/block/blocks.h>
#include <level/block/block_sound.h>
#include <level/level.h>

block_t block_liquid_create(int block_type, int liquid_type) {
    block_t block = block_create(block_type, liquid_type == LIQUID_WATER ? TEXTURE_WATER : TEXTURE_LAVA, block_sounds.none, 1, 100, 1);
    block_set_bounds(&block, 0.01, -0.1 + 0.01, 0.01, 1.01, 0.9 + 0.01, 1.0 + 0.01);
    block.liquid_type = liquid_type;
    block.liquid_moving_id = block_type;
    block.liquid_still_id = block_type + 1;
    block.is_solid = 0;
    block.is_opaque = 1;
    block.is_cube = 0;
    block.tick_delay = liquid_type == LIQUID_LAVA ? 5 : 0;
    block.render_pass = liquid_type == LIQUID_LAVA ? 0 : 1;
    block.has_physics = 1;
    block.should_tick = 0;

    block.on_placed = block_liquid_on_placed;
    block.update = block_liquid_update;
    block.get_brightness = block_liquid_get_brightness;
    block.can_render_side = block_liquid_can_render_side;
    block.render_inside = block_liquid_render_inside;
    block.on_neighbor_changed = block_liquid_on_neighbor_changed;
    block.get_collision_aabb = block_liquid_get_collision_aabb;

    block_list[block.id] = block;

    return block;
}

void block_liquid_on_placed(block_t *block, struct level_s *level, int x, int y, int z) {
    level_t *real_level = (level_t *)level;
    level_add_to_next_tick(real_level, x, y, z, block->liquid_moving_id);
}

uint8_t can_flow(block_t *block, level_t *level, int x, int y, int z) {
    if(block->liquid_type == LIQUID_WATER) {
        for(int i = x - 2; i <= x + 2; i++) {
            for(int j = y - 2; j <= y + 2; j++) {
                for(int k = z - 2; k <= z + 2; k++) {
                    if(level_get_block(level, i, j, k) == blocks.sponge.id) return 0;
                }
            }
        }
    }

    return 1;
}

uint8_t flow(block_t *block, level_t *level, int x, int y, int z) {
    level_t *real_level = (level_t *)level;
    if(level_get_block(real_level, x, y, z) == blocks.air.id) {
        if(!can_flow(block, real_level, x, y, z)) return 0;
        if(level_set_block(real_level, x, y, z, block->liquid_moving_id)) {
            level_add_to_next_tick(real_level, x, y, z, block->liquid_moving_id);
        }
    }
    return 0;
}

void block_liquid_update(block_t *block, struct level_s *level, int x, int y, int z, random_t *random) {
    level_t *real_level = (level_t *)level;
    uint8_t set = 0;
    uint8_t can_set = 0;
    do {
        y--;
        if(level_get_block(real_level, x, y, z) != blocks.air.id || !can_flow(block, real_level, x, y, z)) {
            break;
        }
        can_set = level_set_block(real_level, x, y, z, block->liquid_moving_id);
        if(can_set) set = 1;
    }while(can_set && block->liquid_type != LIQUID_LAVA);

    y++;
    if(block->liquid_type == LIQUID_WATER || !set) {
        set = set | flow(block, real_level, x - 1, y, z) | flow(block, real_level, x + 1, y, z) | flow(block, real_level, x, y, z - 1) | flow(block, real_level, x, y, z + 1);
    }
    if(!set) {
        level_set_block_no_update(real_level, x, y, z, block->liquid_still_id);
    }else {
        level_add_to_next_tick(real_level, x, y, z, block->liquid_moving_id);
    }
}

float block_liquid_get_brightness(block_t *block, struct level_s *level, int x, int y, int z) {
    level_t *real_level = (level_t *)level;
    return block->liquid_type == LIQUID_LAVA ? 100 : level_get_brightness(real_level, x, y, z);
}

uint8_t block_liquid_can_render_side(block_t *block, struct level_s *level, int x, int y, int z, int side) {
    level_t *real_level = (level_t *)level;
    if(x >= 0 && y >= 0 && z >= 0 && x < (int)real_level->width && z < real_level->height) {
		uint8_t tile = level_get_block(real_level, x, y, z);
		if(tile != block->liquid_moving_id && tile != block->liquid_still_id) {
			if(side == 1) {
				uint8_t should_render = level_get_block(real_level, x, y, z) == 0;
				should_render |= level_get_block(real_level, x - 1, y, z) == 0;
				should_render |= level_get_block(real_level, x + 1, y, z) == 0;
				should_render |= level_get_block(real_level, x, y, z - 1) == 0;
				should_render |= level_get_block(real_level, x, y, z + 1) == 0;
				should_render |= level_get_block(real_level, x - 1, y, z - 1) == 0;
				should_render |= level_get_block(real_level, x + 1, y, z - 1) == 0;
				should_render |= level_get_block(real_level, x - 1, y, z + 1) == 0;
				should_render |= level_get_block(real_level, x + 1, y, z + 1) == 0;
				return should_render;
			} else {
				return !level_is_solid_block(real_level, x, y, z);
			}
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

void block_liquid_render_inside(block_t *block, int x, int y, int z, int side) {
    block_render_side_with_texture(block, x, y, z, side, block->get_texture_id(block, side));
    block_render_side(block, x, y, z, side);
}

void block_liquid_on_neighbor_changed(block_t *block, struct level_s *level, int x, int y, int z, uint8_t block_id) {
    level_t *real_level = (level_t *)level;
    if(block_id != blocks.air.id) {
        uint8_t liquid_type = block_list[block_id].liquid_type;
        if((block->liquid_type == LIQUID_WATER && liquid_type == LIQUID_LAVA) || (block->liquid_type == LIQUID_LAVA && liquid_type == LIQUID_WATER)) {
            level_set_block(real_level, x, y, z, blocks.cobblestone.id);
            return;
        }
    }
    level_add_to_next_tick(real_level, x, y, z, block_id);
}

AABB_t block_liquid_get_collision_aabb(block_t *block, int x, int y, int z) {
    return (AABB_t){ .null = 1 };
}