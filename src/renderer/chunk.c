#include <renderer/chunk.h>
#include <renderer/tesselator.h>
#include <level/block/blocks.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include <stdio.h>

int chunk_updates = 0;

chunk_t chunk_create(level_t *level, int x, int y, int z, int chunk_size, int base_list_id) {
	chunk_t chunk = { 0 };
	chunk.visible = 0;
	chunk.level = level;
	chunk.x = x;
	chunk.y = y;
	chunk.z = z;
	chunk.width = chunk_size;
	chunk.height = chunk_size;
	chunk.depth = chunk_size;
	chunk.base_list_id = base_list_id;

	chunk_set_all_dirty(&chunk);

	return chunk;
}

void chunk_update(chunk_t *chunk) {
	chunk_updates++;
	int x0 = chunk->x;
	int y0 = chunk->y;
	int z0 = chunk->z;
	int x1 = chunk->x + chunk->width;
	int y1 = chunk->y + chunk->height;
	int z1 = chunk->z + chunk->depth;

	for(int i = 0; i < 2; i++) {
		chunk->dirty[i] = 1;
	}
	for(int i = 0; i < 2; i++) {
		uint8_t b0 = 0;
		uint8_t b1 = 0;
		glNewList(chunk->base_list_id + i, GL_COMPILE);
		tesselator_begin();
		for(int x = x0; x < x1; x++) {
			for(int y = y0; y < y1; y++) {
				for(int z = z0; z < z1; z++) {
					uint8_t block_id = level_get_block(chunk->level, x, y, z);
					if(block_id != blocks.air.id) {
						block_t *block = &block_list[block_id];
						if(block->render_pass != i) b0 = 1;
						else b1 |= block->render(block, (struct level_s *)chunk->level, x, y, z);
					}
				}
			}
		}
		tesselator_end();
		glEndList();
		if(b1) chunk->dirty[i] = 0;
		if(!b0) break;
	}
}

float chunk_distance_squared(chunk_t *chunk, player_t *player) {
	return (player->x - chunk->x) * (player->x - chunk->x) + (player->y - chunk->y) * (player->y - chunk->y) + (player->z - chunk->z) * (player->z - chunk->z);
}

void chunk_set_all_dirty(chunk_t *chunk) {
	for (int i = 0; i < 2; i++) {
		chunk->dirty[i] = 1;
	}
}

void chunk_dispose(chunk_t *chunk) {
	chunk_set_all_dirty(chunk);
}

int chunk_append_lists(chunk_t *chunk, int data_cache[], int count, int pass) {
	if(!chunk->visible) return count;
	if(!chunk->dirty[pass]) {
		data_cache[count++] = chunk->base_list_id + pass;
	}
	return count;
}

void chunk_clip(chunk_t *chunk, frustum_t frustum) {
	chunk->visible = frustum_contains_box(frustum, chunk->x, chunk->y, chunk->z, chunk->x + chunk->width, chunk->y + chunk->height, chunk->z + chunk->depth);
}

int chunk_list_comparator(const void *a, const void *b) {
    chunk_t *ca = *(chunk_t **)a;
    chunk_t *cb = *(chunk_t **)b;

    float apx = ca->level->player->x, apy = ca->level->player->y, apz = ca->level->player->z;
	float bpx = cb->level->player->x, bpy = cb->level->player->y, bpz = cb->level->player->z;
	float distA = (ca->x - apx) * (ca->x - apx) + (ca->y - apy) * (ca->y - apy) + (ca->z - apz) * (ca->z - apz);
	float distB = (cb->x - bpx) * (cb->x - bpx) + (cb->y - bpy) * (cb->y - bpy) + (cb->z - bpz) * (cb->z - bpz);
	return distA == distB ? 0 : (distA > distB ? 1 : -1);
}

int chunk_visible_distance_comparator(const void *a, const void *b) {
	chunk_t *ca = *(chunk_t **)a;
	chunk_t *cb = *(chunk_t **)b;
	if (ca->visible || !cb->visible) {
		if (cb->visible) {
			float apx = ca->level->player->x, apy = ca->level->player->y, apz = ca->level->player->z;
			float bpx = cb->level->player->x, bpy = cb->level->player->y, bpz = cb->level->player->z;
			float distA = (ca->x - apx) * (ca->x - apx) + (ca->y - apy) * (ca->y - apy) + (ca->z - apz) * (ca->z - apz);
			float distB = (cb->x - bpx) * (cb->x - bpx) + (cb->y - bpy) * (cb->y - bpy) + (cb->z - bpz) * (cb->z - bpz);
			return distA == distB ? 0 : (distA > distB ? 1 : -1);
		} else return -1;
	} else return 1;
}