#include <level/block/block_sponge.h>
#include <level/block/blocks.h>
#include <level/block/block_sound.h>
#include <level/level.h>

block_t block_sponge_create() {
    block_t block = block_create(BLOCK_SPONGE, TEXTURE_SPONGE, block_sounds.wool, 0.9, 0.6, 1);
    block.on_added = block_sponge_on_added;
    block.on_removed = block_sponge_on_removed;

	block_list[block.id] = block;

    return block;
}

void block_sponge_on_added(block_t *block, struct level_s *level, int x, int y, int z) {
    level_t *real_level = (level_t *)level;
    for(int i = x - 2; i <= x + 2; i++) {
		for(int j = y - 2; j <= y + 2; j++) {
			for(int k = z - 2; k <= z + 2; k++) {
				if(level_is_water(real_level, i, j, k)) level_set_block_no_neighbor_change(real_level, i, j, k, blocks.air.id);
			}
		}
	}
}

void block_sponge_on_removed(block_t *block, struct level_s *level, int x, int y, int z) {
    level_t *real_level = (level_t *)level;
    for(int i = x - 2; i <= x + 2; i++) {
		for(int j = y - 2; j <= y + 2; j++) {
			for(int k = z - 2; k <= z + 2; k++) {
				level_update_neighbors_at(real_level, i, j, k, level_get_block(real_level, i, j, k));
			}
		}
	}
}