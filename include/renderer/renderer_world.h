#pragma once

#include <item/item_stack.h>
#include <player/player.h>
#include <renderer/frustum.h>
#include <renderer/renderer_block.h>
#include <renderer/renderer_chunk.h>
#include <renderer/textures.h>

struct minecraft_s;
struct world_s;

typedef struct renderer_world_s {
    struct world_s *world;
    struct minecraft_s *minecraft;
    textures_t *textures;
    int list_id;
    renderer_chunk_t **renderer_chunks_sorted;
    renderer_chunk_t **renderer_chunks;
    uint64_t *renderer_chunks_to_update; // array_list
    int renderer_chunk_count;
    int chunk_cache_count;
    int x_chunks;
    int y_chunks;
    int z_chunks;
    int render_list_base;
    int *render_lists; // array_list
    uint32_t occlusion_query_base[262144];
    uint32_t occlusion_result[64];
    int *chunk_data_cache;
    int ticks;
    float last_load_x;
    float last_load_y;
    float last_load_z;
    float destroy_progress;
    int cloud_offset_x;
    uint8_t occlusion_enabled;
    int star_render_list;
    int sky_render_list;
    renderer_block_t renderer_block;
    int render_distance;
    int x0, y0, z0;
    int x1, y1, z1;
    int entities_total;
    int entities_rendered;
    int entities_hidden;
    int renderers_loaded;
    int renderers_clipped;
    int renderers_occluded;
    int renderers_rendered;
} renderer_world_t;

renderer_world_t renderer_world_create(struct minecraft_s *minecraft, struct world_s *world, textures_t *textures);
void renderer_world_change_world(renderer_world_t *renderer, struct world_s *world);
void renderer_world_load_renderers(renderer_world_t *renderer);
void renderer_world_update_entities(renderer_world_t *renderer, vec3_t pos, frustum_t *frustum, float partial_tick);
void renderer_world_new_position(renderer_world_t *renderer, int x, int y, int z);
int renderer_world_sort_and_render(renderer_world_t *renderer, entity_t *player, int render_pass, double partial_tick);
void renderer_world_check_occlusion(renderer_world_t *renderer, int start, int end);
int renderer_world_render_sorted_renderers(renderer_world_t *renderer, int start, int end, int render_pass, double partial_tick);
void renderer_world_render_all_lists(renderer_world_t *renderer, int render_pass, double partial_tick);
void renderer_world_update_clouds(renderer_world_t *renderer);
void renderer_world_draw_sky(renderer_world_t *renderer, float partial_tick);
void renderer_world_update_renderers(renderer_world_t *renderer, entity_t *player);
void renderer_world_draw_block_breaking(renderer_world_t *renderer, entity_t *player, hit_result_t *hit_result, int unused, item_stack_t *item_stack, float partial_tick);
void renderer_world_draw_selection_box(renderer_world_t *renderer, entity_t *player, hit_result_t *hit_result, uint8_t block_id, float partial_tick);
void renderer_world_queue_chunks(renderer_world_t *renderer, int x0, int y0, int z0, int x1, int y1, int z1);
void renderer_world_update_block(renderer_world_t *renderer, int x, int y, int z);
void renderer_world_update_blocks(renderer_world_t *renderer, int x0, int y0, int z0, int x1, int y1, int z1);
void renderer_world_update_frustum(renderer_world_t *renderer, frustum_t *frustum);
void renderer_world_play_sound(renderer_world_t *renderer, uint8_t sound, double x, double y, double z, float volume, float pitch);
void renderer_world_spawn_particle(renderer_world_t *renderer, uint8_t particle_type, double x, double y, double z, double x_vel, double y_vel, double z_vel);
void renderer_world_obtain_entity_skin(renderer_world_t *renderer, entity_t *entity);
void renderer_world_release_entity_skin(renderer_world_t *renderer, entity_t *entity);
void renderer_world_update_all(renderer_world_t *renderer);
void renderer_world_destroy(renderer_world_t *renderer);