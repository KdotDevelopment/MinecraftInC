#include <renderer/renderer_world.h>

#include <renderer/tesselator.h>
#include <world/block/blocks.h>
#include <world/world.h>
#include <minecraft.h>

#include <util/array_list.h>
#include <util/time.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL_opengl.h>

#include <limits.h>
#include <stdio.h>

void renderer_world_create(renderer_world_t *renderer, struct minecraft_s *minecraft, world_t *world, textures_t *textures) {
    memset(renderer, 0, sizeof(renderer_world_t));

    renderer->minecraft = minecraft;
    renderer->ticks = 0;
    renderer->last_load_x = -9999;
    renderer->last_load_y = -9999;
    renderer->last_load_z = -9999;
    renderer->textures = textures;
    renderer->list_id = glGenLists(2);
    renderer->render_list_base = glGenLists(786432);
    renderer->world = world;
    renderer->renderer_chunks_to_update = array_list_create(sizeof(renderer_chunk_t *));
    renderer->render_lists = array_list_create(sizeof(renderer_chunk_t *));

    if(renderer->occlusion_enabled) {
        memset(renderer->occlusion_query_base, 0, sizeof(renderer->occlusion_query_base));
        glGenQueriesARB(262144, renderer->occlusion_query_base);
    }

    // Star renderer
    renderer->star_render_list = glGenLists(1);
    glNewList(renderer->star_render_list, GL_COMPILE);
    // fun fact: this seed dictates the star formation
    random_t random = random_create(10842);

    for(int i = 0; i < 500; i++) {
        glRotatef(random_next_uniform(&random) * 360.0, 1.0, 0.0, 0.0);
        glRotatef(random_next_uniform(&random) * 360.0, 0.0, 1.0, 0.0);
        glRotatef(random_next_uniform(&random) * 360.0, 0.0, 0.0, 1.0);

        float star_size = 0.25 + random_next_uniform(&random) * 0.25;

        tesselator_begin_quads();
        tesselator_vertex_uv(-star_size, -100.0, star_size, 1.0, 1.0);
        tesselator_vertex_uv(star_size, -100.0, star_size, 0.0, 1.0);
        tesselator_vertex_uv(star_size, -100.0, -star_size, 0.0, 0.0);
        tesselator_vertex_uv(-star_size, -100.0, -star_size, 1.0, 0.0);
        tesselator_end();
    }

    glEndList();

    // Sky renderer
    renderer->sky_render_list = glGenLists(1);
    glNewList(renderer->sky_render_list, GL_COMPILE);
    tesselator_begin_quads();

    for(int x = -256; x <= 256; x += 32) {
        for(int y = -256; y <= 256; y += 32) {
            tesselator_vertex(x, 16.0, y);
            tesselator_vertex(x + 32.0, 16.0, y);
            tesselator_vertex(x + 32.0, 16.0, y + 32.0);
            tesselator_vertex(x, 16.0, y + 32.0);
        }
    }

    tesselator_end();
    glEndList();
}

void renderer_world_change_world(renderer_world_t *renderer, world_t *world) {
    if(renderer->world != NULL) {
        world_remove_renderer(renderer->world, renderer);
    }

    renderer->last_load_x = -9999;
    renderer->last_load_y = -9999;
    renderer->last_load_z = -9999;
    // rendermanager set world
    renderer->renderer_block = renderer_block_create(world);
    if(world != NULL) {
        world_add_renderer(world, renderer);
        renderer_world_load_renderers(renderer);
    }
}

void renderer_world_load_renderers(renderer_world_t *renderer) {
    renderer->render_distance = renderer->minecraft->settings.view_distance;
    if(renderer->renderer_chunks != NULL) {
        for(int i = 0; i < renderer->renderer_chunk_count; i++) {
            renderer_chunk_t *renderer_chunk = (renderer_chunk_t *)renderer->renderer_chunks[i];
            renderer_chunk_stop_rendering(renderer_chunk);
            free(renderer_chunk);
        }
    }

    free(renderer->renderer_chunks);
    free(renderer->renderer_chunks_sorted);

    int distance = 5 << (3 - renderer->render_distance);
    if(distance > 28) {
        distance = 28;
    }

    renderer->x_chunks = distance;
    renderer->y_chunks = 8;
    renderer->z_chunks = distance;
    renderer->renderer_chunks = malloc(renderer->x_chunks * renderer->y_chunks * renderer->z_chunks * sizeof(renderer_chunk_t *));
    renderer->renderer_chunks_sorted = malloc(renderer->x_chunks * renderer->y_chunks * renderer->z_chunks * sizeof(renderer_chunk_t *));
    renderer->renderer_chunk_count = renderer->x_chunks * renderer->y_chunks * renderer->z_chunks;
    renderer->x0 = 0;
    renderer->y0 = 0;
    renderer->z0 = 0;
    renderer->x1 = renderer->x_chunks;
    renderer->y1 = renderer->y_chunks;
    renderer->z1 = renderer->z_chunks;

    /*for(int i = 0; i < array_list_length(renderer->renderer_chunks_to_update); i++) {
        renderer_chunk_t *renderer_chunk = *(renderer_chunk_t **)array_list_get(renderer->renderer_chunks_to_update, i);
        renderer_chunk->needs_update = 0;
    }*/

    renderer->renderer_chunks_to_update = array_list_clear(renderer->renderer_chunks_to_update);

    int render_list_index = 0;
    int chunk_index = 0;

    for(int x = 0; x < renderer->x_chunks; x++) {
        for(int y = 0; y < renderer->y_chunks; y++) {
            for(int z = 0; z < renderer->z_chunks; z++) {
                renderer_chunk_t *renderer_chunk = malloc(sizeof(renderer_chunk_t));
                renderer_chunk_create(renderer_chunk, renderer->world, x * CHUNK_SIZE_WIDTH, y * CHUNK_SIZE_WIDTH, z * CHUNK_SIZE_WIDTH, CHUNK_SIZE_WIDTH, renderer->render_list_base + render_list_index);
                renderer->renderer_chunks[(z * renderer->y_chunks + y) * renderer->x_chunks + x] = renderer_chunk;
                if(renderer->occlusion_enabled) {
                    renderer_chunk->gl_occlusion_query = renderer->occlusion_query_base[chunk_index];
                }
                renderer_chunk->waiting_on_occlusion = 0;
                renderer_chunk->is_visible = 1;
                renderer_chunk->is_in_frustum = 1;
                chunk_index++;
                renderer_chunk->needs_update = 1;
                renderer->renderer_chunks_sorted[(z * renderer->y_chunks + y) * renderer->x_chunks + x] = renderer->renderer_chunks[(z * renderer->y_chunks + y) * renderer->x_chunks + x];
                renderer->renderer_chunks_to_update = array_list_push(renderer->renderer_chunks_to_update, &renderer->renderer_chunks[(z * renderer->y_chunks + y) * renderer->x_chunks + x]);
                render_list_index += 3;
            }
        }
    }

    entity_t *player = &renderer->minecraft->player.mob.entity;
    renderer_world_new_position(renderer, floor_double(player->x), floor_double(player->y), floor_double(player->z));
    qsort(renderer->renderer_chunks_sorted, renderer->renderer_chunk_count, sizeof(renderer_chunk_t *), renderer_chunk_entity_compare);
}

void renderer_world_update_entities(renderer_world_t *renderer, vec3_t pos, frustum_t *frustum, float partial_tick) {
    // render_manager cache active render info
    renderer->entities_total = 0;
    renderer->entities_rendered = 0;
    renderer->entities_hidden = 0;
    entity_t *player = &renderer->world->player->mob.entity;
    // render_manager.x = renderer->minecraft->player.last_tick_x + (renderer->minecraft->player.x - renderer->minecraft->player.last_tick_x) * partial_tick;
    // render_manager.y = renderer->minecraft->player.last_tick_y + (renderer->minecraft->player.y - renderer->minecraft->player.last_tick_y) * partial_tick;
    // render_manager.z = renderer->minecraft->player.last_tick_z + (renderer->minecraft->player.z - renderer->minecraft->player.last_tick_z) * partial_tick;
    entity_t **entities = array_list_clone(renderer->world->loaded_entity_list);
    renderer->entities_total = array_list_length(entities);

    for(int i = 0; i < renderer->entities_total; i++) {
        entity_t *entity = *(entity_t **)array_list_get(entities, i);
        double dx = entity->x - pos.x;
        double dy = entity->y - pos.y;
        double dz = entity->z - pos.z;
        double distance_sq = dx * dx + dy * dy + dz * dz;
        AABB_t bb = entity->bb;
        double size_x = bb.x1 - bb.x0;
        double size_y = bb.y1 - bb.y0;
        double size_z = bb.z1 - bb.z0;
        double avg_size = (size_x + size_y + size_z) / 3.0;
        avg_size *= 64.0;
        
        if(distance_sq < avg_size && frustum_contains_box_bb(*frustum, bb) && (entity != player || renderer->minecraft->settings.third_person)) {
            renderer->entities_rendered++;
            // render_manager render entity (entity, partial_tick)
        }
    }

    array_list_free(entities);
}

void renderer_world_new_position(renderer_world_t *renderer, int x, int y, int z) {
    x -= CHUNK_SIZE_WIDTH / 2;
    z -= CHUNK_SIZE_WIDTH / 2;

    renderer->x0 = INT_MAX;
    renderer->y0 = INT_MAX;
    renderer->z0 = INT_MAX;
    renderer->x1 = INT_MIN;
    renderer->y1 = INT_MIN;
    renderer->z1 = INT_MIN;

    int chunk_size = renderer->x_chunks * CHUNK_SIZE_WIDTH;
    int half_chunk_size = chunk_size / 2;

    for(int chunk_x = 0; chunk_x < renderer->x_chunks; chunk_x++) {
        int chunk_x_pos = chunk_x * CHUNK_SIZE_WIDTH;
        int offset_x = chunk_x_pos + half_chunk_size - x;

        if(offset_x < 0) offset_x -= chunk_size - 1;
        offset_x /= chunk_size;
        chunk_x_pos -= offset_x * chunk_size;

        if(chunk_x_pos < renderer->x0) renderer->x0 = chunk_x_pos;
        if(chunk_x_pos > renderer->x1) renderer->x1 = chunk_x_pos;

        for(int chunk_z = 0; chunk_z < renderer->z_chunks; chunk_z++) {
            int chunk_z_pos = chunk_z * CHUNK_SIZE_WIDTH;
            int offset_z = chunk_z_pos + half_chunk_size - z;

            if(offset_z < 0) offset_z -= chunk_size - 1;
            offset_z /= chunk_size;
            chunk_z_pos -= offset_z * chunk_size;

            if(chunk_z_pos < renderer->z0) renderer->z0 = chunk_z_pos;
            if(chunk_z_pos > renderer->z1) renderer->z1 = chunk_z_pos;

            for(int chunk_y = 0; chunk_y < renderer->y_chunks; chunk_y++) {
                int chunk_y_pos = chunk_y * CHUNK_SIZE_WIDTH;

                if(chunk_y_pos < renderer->y0) renderer->y0 = chunk_y_pos;
                if(chunk_y_pos > renderer->y1) renderer->y1 = chunk_y_pos;

                renderer_chunk_t *chunk_renderer = renderer->renderer_chunks[(chunk_z * renderer->y_chunks + chunk_y) * renderer->x_chunks + chunk_x];
                uint8_t updated = chunk_renderer->needs_update;

                renderer_chunk_set_position(chunk_renderer, chunk_x_pos, chunk_y_pos, chunk_z_pos);

                if(!updated && chunk_renderer->needs_update) {
                    renderer->renderer_chunks_to_update = array_list_push(renderer->renderer_chunks_to_update, chunk_renderer);
                }
            }
        }
    }
}

int renderer_world_sort_and_render(renderer_world_t *renderer, entity_t *player, int render_pass, double partial_tick) {
    if(renderer->minecraft->settings.view_distance != renderer->render_distance) {
        renderer_world_load_renderers(renderer);
    }

    if(render_pass == 0) {
        renderer->renderers_loaded = 0;
        renderer->renderers_clipped = 0;
        renderer->renderers_occluded = 0;
        renderer->renderers_rendered = 0;
    }

    double x = renderer->minecraft->player.last_tick_x + (renderer->minecraft->player.x - renderer->minecraft->player.last_tick_x) * partial_tick;
    double y = renderer->minecraft->player.last_tick_y + (renderer->minecraft->player.y - renderer->minecraft->player.last_tick_y) * partial_tick;
    double z = renderer->minecraft->player.last_tick_z + (renderer->minecraft->player.z - renderer->minecraft->player.last_tick_z) * partial_tick;

    double dx = player->x - renderer->last_load_x;
    double dy = player->y - renderer->last_load_y;
    double dz = player->z - renderer->last_load_z;

    if(dx * dx + dy * dy + dz * dz > 0.01) {
        renderer->last_load_x = player->x;
        renderer->last_load_y = player->y;
        renderer->last_load_z = player->z;
        renderer_world_new_position(renderer, floor_double(player->x), floor_double(player->y), floor_double(player->z));
        qsort(renderer->renderer_chunks_sorted, renderer->renderer_chunk_count, sizeof(renderer_chunk_t *), renderer_chunk_entity_compare);
    }

    int total_rendered = 0;

    if(renderer->occlusion_enabled && !renderer->minecraft->settings.anaglyph && render_pass == 0) {
        int batch_size = 16;
        renderer_world_check_occlusion(renderer, 0, batch_size);

        for(int i = 0; i < batch_size; i++) {
            renderer->renderer_chunks_sorted[i]->is_visible = 1;
        }

        total_rendered += renderer_world_render_sorted_renderers(renderer, 0, batch_size, render_pass, partial_tick);

        do {
            int previous_batch_size = batch_size;
            batch_size *= 2;
            if(batch_size > renderer->renderer_chunk_count) {
                batch_size = renderer->renderer_chunk_count;
            }

            glDisable(GL_TEXTURE_2D);
            glDisable(GL_LIGHTING);
            glDisable(GL_ALPHA_TEST);
            glDisable(GL_FOG);
            glColorMask(0, 0, 0, 0);
            glDepthMask(0);

            renderer_world_check_occlusion(renderer, previous_batch_size, batch_size);

            glPushMatrix();
            float acc_x = 0;
            float acc_y = 0;
            float acc_z = 0;

            for(int i = previous_batch_size; i < batch_size; i++) {
                renderer_chunk_t *chunk_renderer = renderer->renderer_chunks_sorted[i];

                if(renderer_chunk_skip_all_render_passes(chunk_renderer)) {
                    chunk_renderer->is_in_frustum = 0;
                }else {
                    if(!chunk_renderer->is_in_frustum) {
                        chunk_renderer->is_visible = 1;
                    }

                    if(chunk_renderer->is_in_frustum && !chunk_renderer->waiting_on_occlusion) {
                        float distance = sqrt(renderer_chunk_distance_to_entity_squared(chunk_renderer, player));
                        int query_interval = (1.0 + distance / 64.0);

                        if(renderer->cloud_offset_x % query_interval == i % query_interval) {
                            float off_x = chunk_renderer->x_minus - x;
                            float off_y = chunk_renderer->y_minus - y;
                            float off_z = chunk_renderer->z_minus - z;

                            off_x -= acc_x;
                            off_y -= acc_y;
                            off_z -= acc_z;

                            if(off_x != 0 || off_y != 0 || off_z != 0) {
                                glTranslatef(off_x, off_y, off_z);
                                acc_x += off_x;
                                acc_y += off_y;
                                acc_z += off_z;
                            }

                            glBeginQueryARB(GL_SAMPLES_PASSED_ARB, chunk_renderer->gl_occlusion_query);
                            renderer_chunk_call_occlusion_query_list(chunk_renderer);
                            glEndQueryARB(GL_SAMPLES_PASSED_ARB);
                            chunk_renderer->waiting_on_occlusion = 1;
                        }
                    }
                }
            }

            glPopMatrix();
            glColorMask(1, 1, 1, 1);
            glDepthMask(1);
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_ALPHA_TEST);
            glEnable(GL_FOG);

            total_rendered += renderer_world_render_sorted_renderers(renderer, previous_batch_size, batch_size, render_pass, partial_tick);
        }while(batch_size < renderer->renderer_chunk_count);
    }else {
        total_rendered += renderer_world_render_sorted_renderers(renderer, 0, renderer->renderer_chunk_count, render_pass, partial_tick);
    }

    return total_rendered;
}

void renderer_world_check_occlusion(renderer_world_t *renderer, int start, int end) {
    for(int i = start; i < end; i++) {
        renderer_chunk_t *chunk_renderer = renderer->renderer_chunks_sorted[i];
        if(chunk_renderer->waiting_on_occlusion) {
            memset(renderer->occlusion_result, 0, sizeof(renderer->occlusion_result));
            glGetQueryObjectuiv(chunk_renderer->gl_occlusion_query, GL_QUERY_RESULT_AVAILABLE, renderer->occlusion_result);
            if(renderer->occlusion_result[0] != 0) {
                chunk_renderer->waiting_on_occlusion = 0;
                memset(renderer->occlusion_result, 0, sizeof(renderer->occlusion_result));
                glGetQueryObjectuiv(chunk_renderer->gl_occlusion_query, GL_QUERY_RESULT, renderer->occlusion_result);
                chunk_renderer->is_visible = renderer->occlusion_result[0] != 0;
            }
        }
    }
}

int renderer_world_render_sorted_renderers(renderer_world_t *renderer, int start, int end, int render_pass, double partial_tick) {
    renderer->render_lists = array_list_clear(renderer->render_lists);
    int total_rendered = 0;

    for(int i = start; i < end; i++) {
        renderer_chunk_t *chunk_renderer = renderer->renderer_chunks_sorted[i];
        if(render_pass == 0) {
            renderer->renderers_loaded++;
            if(!chunk_renderer->is_in_frustum) renderer->renderers_clipped++;
            if(chunk_renderer->is_in_frustum && !chunk_renderer->is_visible) renderer->renderers_occluded++;
            if(chunk_renderer->is_in_frustum && chunk_renderer->is_visible) renderer->renderers_rendered++;
        }

        if(chunk_renderer->is_in_frustum && chunk_renderer->is_visible) {
            int rendered = renderer_chunk_get_gl_call_list(chunk_renderer, render_pass);
            if(rendered >= 0) {
                renderer->render_lists = array_list_push(renderer->render_lists, &renderer->renderer_chunks_sorted[i]);
                total_rendered++;
            }
        }
    }

    renderer_world_render_all_lists(renderer, render_pass, partial_tick);
    return total_rendered;
}

void renderer_world_render_all_lists(renderer_world_t *renderer, int render_pass, double partial_tick) {
    double x = renderer->minecraft->player.last_tick_x + (renderer->minecraft->player.x - renderer->minecraft->player.last_tick_x) * partial_tick;
    double y = renderer->minecraft->player.last_tick_y + (renderer->minecraft->player.y - renderer->minecraft->player.last_tick_y) * partial_tick;
    double z = renderer->minecraft->player.last_tick_z + (renderer->minecraft->player.z - renderer->minecraft->player.last_tick_z) * partial_tick;
    glPushMatrix();

    float acc_x = 0;
    float acc_y = 0;
    float acc_z = 0;

    for(int i = 0; i < array_list_length(renderer->render_lists); i++) {
        renderer_chunk_t *chunk_renderer = *(renderer_chunk_t **)array_list_get(renderer->render_lists, i);
        float off_x = chunk_renderer->x_minus - x;
        float off_y = chunk_renderer->y_minus - y;
        float off_z = chunk_renderer->z_minus - z;

        off_x -= acc_x;
        off_y -= acc_y;
        off_z -= acc_z;

        if(off_x != 0 || off_y != 0 || off_z != 0) {
            glTranslatef(off_x, off_y, off_z);
            acc_x += off_x;
            acc_y += off_y;
            acc_z += off_z;
        }

        glCallList(renderer_chunk_get_gl_call_list(chunk_renderer, render_pass));
    }

    glPopMatrix();
}

void renderer_world_update_clouds(renderer_world_t *renderer) {
    renderer->cloud_offset_x++;
}

void renderer_world_draw_sky(renderer_world_t *renderer, float partial_tick) {
    glDisable(GL_TEXTURE_2D);
    double dx = renderer->minecraft->player.xo + (renderer->minecraft->player.x - renderer->minecraft->player.xo) * partial_tick + (renderer->cloud_offset_x + partial_tick) * 0.03;
    double dy = renderer->minecraft->player.yo + (renderer->minecraft->player.y - renderer->minecraft->player.yo) * partial_tick;
    double dz = renderer->minecraft->player.zo + (renderer->minecraft->player.z - renderer->minecraft->player.zo) * partial_tick;
    
    vec3_t sky_color = world_get_sky_color(renderer->world, partial_tick);
    float sky_r = sky_color.x;
    float sky_g = sky_color.y;
    float sky_b = sky_color.z;
    if(renderer->minecraft->settings.anaglyph) {
        float r = (sky_r * 30.0 + sky_g * 59.0 + sky_b * 11.0) / 100.0;
        float g = (sky_r * 30.0 + sky_g * 70.0) / 100.0;
        float b = (sky_r * 30.0 + sky_b * 70.0) / 100.0;
        sky_r = r;
        sky_g = g;
        sky_b = b;
    }

    glColor3f(sky_r, sky_g, sky_b);
    glDepthMask(GL_FALSE);
    glCallList(renderer->sky_render_list);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_FOG);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glPushMatrix();
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glTranslatef(0.0, 0.0, 0.0);
    glRotatef(0.0, 0.0, 0.0, 1.0);
    glRotatef(world_get_celestial_angle(renderer->world, partial_tick) * 360.0, 1.0, 0.0, 0.0);

    glBindTexture(GL_TEXTURE_2D, textures_load(renderer->textures, "terrain/sun.png"));
    tesselator_begin_quads();
    tesselator_vertex_uv(-30.0, 100.0, -30.0, 0.0, 0.0);
    tesselator_vertex_uv(30.0, 100.0, -30.0, 1.0, 0.0);
    tesselator_vertex_uv(30.0, 100.0, 30.0, 1.0, 1.0);
    tesselator_vertex_uv(-30.0, 100.0, 30.0, 0.0, 1.0);
    tesselator_end();

    glBindTexture(GL_TEXTURE_2D, textures_load(renderer->textures, "terrain/moon.png"));
    tesselator_begin_quads();
    tesselator_vertex_uv(-20.0, -100.0, 20.0, 1.0, 1.0);
    tesselator_vertex_uv(20.0, -100.0, 20.0, 0.0, 1.0);
    tesselator_vertex_uv(20.0, -100.0, -20.0, 0.0, 0.0);
    tesselator_vertex_uv(-20.0, -100.0, -20.0, 1.0, 0.0);
    tesselator_end();

    glDisable(GL_TEXTURE_2D);
    float brightness = world_get_star_brightness(renderer->world, partial_tick);
    glColor4f(brightness, brightness, brightness, brightness);
    glCallList(renderer->star_render_list);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_FOG);
    glPopMatrix();
    glDepthMask(GL_TRUE);

    glBindTexture(GL_TEXTURE_2D, textures_load(renderer->textures, "clouds.png"));
    glColor4f(1.0, 1.0, 1.0, 1.0);
    vec3_t cloud_color = world_get_cloud_color(renderer->world, partial_tick);
    float cloud_r = cloud_color.x;
    float cloud_g = cloud_color.y;
    float cloud_b = cloud_color.z;
    if(renderer->minecraft->settings.anaglyph) {
        float r = (cloud_r * 30.0 + cloud_g * 59.0 + cloud_b * 11.0) / 100.0;
        float g = (cloud_r * 30.0 + cloud_g * 70.0) / 100.0;
        float b = (cloud_r * 30.0 + cloud_b * 70.0) / 100.0;
        cloud_r = r;
        cloud_g = g;
        cloud_b = b;
    }

    int cloud_x = floor_double(dx / 2048.0);
    int cloud_z = floor_double(dz / 2048.0);
    dx -= cloud_x << 11;
    dz -= cloud_z << 11;
    float height = 120.0 - dy + 0.33;
    float texture_u = (dx * (0.5 / 1024.0));
    float texture_v = (dz * (0.5 / 1024.0));
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    tesselator_begin_quads();
    tesselator_color_float(cloud_r, cloud_g, cloud_b, 1.0);

    for(int grid_x = -256; grid_x < 256; grid_x += 32) {
        for(int grid_z = -256; grid_z < 256; grid_z += 32) {
            tesselator_vertex_uv(grid_x,      height, grid_z + 32,  grid_x       * (0.5 / 1024.0) + texture_u, (grid_z + 32) * (0.5 / 1024.0) + texture_v);
            tesselator_vertex_uv(grid_x + 32, height, grid_z + 32, (grid_x + 32) * (0.5 / 1024.0) + texture_u, (grid_z + 32) * (0.5 / 1024.0) + texture_v);
            tesselator_vertex_uv(grid_x + 32, height, grid_z,      (grid_x + 32) * (0.5 / 1024.0) + texture_u,  grid_z       * (0.5 / 1024.0) + texture_v);
            tesselator_vertex_uv(grid_x,      height, grid_z,       grid_x       * (0.5 / 1024.0) + texture_u,  grid_z       * (0.5 / 1024.0) + texture_v);

            tesselator_vertex_uv(grid_x,      height, grid_z,       grid_x       * (0.5 / 1024.0) + texture_u,  grid_z       * (0.5 / 1024.0) + texture_v);
            tesselator_vertex_uv(grid_x + 32, height, grid_z,      (grid_x + 32) * (0.5 / 1024.0) + texture_u,  grid_z       * (0.5 / 1024.0) + texture_v);
            tesselator_vertex_uv(grid_x + 32, height, grid_z + 32, (grid_x + 32) * (0.5 / 1024.0) + texture_u, (grid_z + 32) * (0.5 / 1024.0) + texture_v);
            tesselator_vertex_uv(grid_x,      height, grid_z + 32,  grid_x       * (0.5 / 1024.0) + texture_u, (grid_z + 32) * (0.5 / 1024.0) + texture_v);
        }
    }

    tesselator_end();
}

void renderer_world_update_renderers(renderer_world_t *renderer, entity_t *player) {
    qsort(renderer->renderer_chunks_to_update, array_list_length(renderer->renderer_chunks_to_update), sizeof(renderer_chunk_t *), renderer_chunk_player_compare);

    int last_index = array_list_length(renderer->renderer_chunks_to_update) - 1;
    int total_renderers = last_index + 1;

    for(int i = 0; i < total_renderers; i++) {
        renderer_chunk_t *chunk_renderer = *(renderer_chunk_t **)array_list_get(renderer->renderer_chunks_to_update, last_index - i);
        if(renderer_chunk_distance_to_entity_squared(chunk_renderer, player) > 2500.0 && i > 2) {
            return;
        }
        array_list_remove(renderer->renderer_chunks_to_update, last_index - i);
        renderer_chunk_update(chunk_renderer);
        chunk_renderer->needs_update = 0;
    }
}

void renderer_world_draw_block_breaking(renderer_world_t *renderer, entity_t *player, hit_result_t *hit_result, int unused, item_stack_t *item_stack, float partial_tick) {
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(1.0, 1.0, 1.0, (tsin(time_millis() / 100.0) * 0.2 + 0.4) * 0.5);
    if(renderer->destroy_progress > 0) {
        glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
        glBindTexture(GL_TEXTURE_2D, textures_load(renderer->textures, "terrain.png"));
        glColor4f(1, 1, 1, 0.5);
        glPushMatrix();
        uint8_t block_id = world_get_block(renderer->world, hit_result->x, hit_result->y, hit_result->z);
        block_t *block = NULL;
        if(block_id != blocks.air.id) {
            block = &block_list[block_id];
        }
        glDisable(GL_ALPHA_TEST);
        glPolygonOffset(-1.0, -1.0);
        glEnable(GL_POLYGON_OFFSET_FILL);
        tesselator_begin_quads();
        double x = player->last_tick_x + (player->x - player->last_tick_x) * partial_tick;
        double y = player->last_tick_y + (player->y - player->last_tick_y) * partial_tick;
        double z = player->last_tick_z + (player->z - player->last_tick_z) * partial_tick;
        tesselator_set_translation(-x, -y, -z);
        tesselator_disable_color();
        if(block == NULL) block = &block_list[blocks.stone.id];
        renderer_block_render_using_texture(&renderer->renderer_block, block, hit_result->x, hit_result->y, hit_result->z, 240 + (renderer->destroy_progress * 10.0));
        tesselator_end();
        tesselator_set_translation(0, 0, 0);
        glPolygonOffset(0, 0);
        glDisable(GL_POLYGON_OFFSET_FILL);
        glEnable(GL_ALPHA_TEST);
        glDepthMask(GL_TRUE);
        glPopMatrix();
    }

    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
}

void renderer_world_draw_selection_box(renderer_world_t *renderer, entity_t *player, hit_result_t *hit_result, uint8_t block_id, float partial_tick) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0, 0.0, 0.0, 0.4);
    glLineWidth(2.0);
    glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_FALSE);
    block_id = world_get_block(renderer->world, hit_result->x, hit_result->y, hit_result->z);
    block_t *block = &block_list[block_id];
    if(block_id > 0) {
        double x = player->last_tick_x + (player->x - player->last_tick_x) * partial_tick;
        double y = player->last_tick_y + (player->y - player->last_tick_y) * partial_tick;
        double z = player->last_tick_z + (player->z - player->last_tick_z) * partial_tick;
        AABB_t bb = AABB_grow(block->get_selection_aabb(block, hit_result->x, hit_result->y, hit_result->z), 0.002, 0.002, 0.002);
        bb = AABB_move(bb, -x, -y, -z);
        tesselator_begin(GL_LINE_STRIP);
        tesselator_vertex(bb.x0, bb.y0, bb.z0);
        tesselator_vertex(bb.x1, bb.y0, bb.z0);
        tesselator_vertex(bb.x1, bb.y0, bb.z1);
        tesselator_vertex(bb.x0, bb.y0, bb.z1);
        tesselator_vertex(bb.x0, bb.y0, bb.z0);
        tesselator_end();
        tesselator_begin(GL_LINE_STRIP);
        tesselator_vertex(bb.x0, bb.y1, bb.z0);
        tesselator_vertex(bb.x1, bb.y1, bb.z0);
        tesselator_vertex(bb.x1, bb.y1, bb.z1);
        tesselator_vertex(bb.x0, bb.y1, bb.z1);
        tesselator_vertex(bb.x0, bb.y1, bb.z0);
        tesselator_end();
        tesselator_begin(GL_LINES);
        tesselator_vertex(bb.x0, bb.y0, bb.z0);
        tesselator_vertex(bb.x0, bb.y1, bb.z0);
        tesselator_vertex(bb.x1, bb.y0, bb.z0);
        tesselator_vertex(bb.x1, bb.y1, bb.z0);
        tesselator_vertex(bb.x1, bb.y0, bb.z1);
        tesselator_vertex(bb.x1, bb.y1, bb.z1);
        tesselator_vertex(bb.x0, bb.y0, bb.z1);
        tesselator_vertex(bb.x0, bb.y1, bb.z1);
        tesselator_end();
    }
    glDepthMask(GL_TRUE);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void renderer_world_queue_chunks(renderer_world_t *renderer, int x0, int y0, int z0, int x1, int y1, int z1) {
    x0 /= CHUNK_SIZE_WIDTH;
    y0 /= CHUNK_SIZE_WIDTH;
    z0 /= CHUNK_SIZE_WIDTH;
    x1 /= CHUNK_SIZE_WIDTH;
    y1 /= CHUNK_SIZE_WIDTH;
    z1 /= CHUNK_SIZE_WIDTH;
    
    for(int x = x0; x <= x1; x++) {
        int wrapped_x = x % renderer->x_chunks;
        if(wrapped_x < 0) wrapped_x += renderer->x_chunks;

        for(int y = y0; y <= y1; y++) {
            int wrapped_y = y % renderer->y_chunks;
            if(wrapped_y < 0) wrapped_y += renderer->y_chunks;

            for(int z = z0; z <= z1; z++) {
                int wrapped_z = z % renderer->z_chunks;
                if(wrapped_z < 0) wrapped_z += renderer->z_chunks;

                renderer_chunk_t *renderer_chunk = renderer->renderer_chunks[(wrapped_z * renderer->y_chunks + wrapped_y) * renderer->x_chunks + wrapped_x];
                if(!renderer_chunk->needs_update) {
                    renderer_chunk->needs_update = 1;
                    renderer->renderer_chunks_to_update = array_list_push(renderer->renderer_chunks_to_update, &renderer_chunk);
                }
            }
        }
    }
}

void renderer_world_update_block(renderer_world_t *renderer, int x, int y, int z) {
    renderer_world_queue_chunks(renderer, x - 1, y - 1, z - 1, x + 1, y + 1, z + 1);
}

void renderer_world_update_blocks(renderer_world_t *renderer, int x0, int y0, int z0, int x1, int y1, int z1) {
    renderer_world_queue_chunks(renderer, x0 - 1, y0 - 1, z0 - 1, x1 + 1, y1 + 1, z1 + 1);
}

void renderer_world_update_frustum(renderer_world_t *renderer, frustum_t *frustum) {
    for(int i = 0; i < renderer->renderer_chunk_count; i++) {
        renderer_chunk_t *renderer_chunk = renderer->renderer_chunks[i];
        renderer_chunk_update_frustum(renderer_chunk, frustum);
    }
}

void renderer_world_play_sound(renderer_world_t *renderer, uint8_t sound, double x, double y, double z, float volume, float pitch) {

}

void renderer_world_spawn_particle(renderer_world_t *renderer, uint8_t particle_type, double x, double y, double z, double x_vel, double y_vel, double z_vel) {
    
}

void renderer_world_obtain_entity_skin(renderer_world_t *renderer, entity_t *entity) {

}

void renderer_world_release_entity_skin(renderer_world_t *renderer, entity_t *entity) {

}

void renderer_world_update_all(renderer_world_t *renderer) {
    for(int i = 0; i < renderer->renderer_chunk_count; i++) {
        renderer_chunk_t *chunk_renderer = renderer->renderer_chunks[i];
        if(!chunk_renderer->needs_update && chunk_renderer->is_lit) {
            chunk_renderer->needs_update = 1;
            renderer->renderer_chunks_to_update = array_list_push(renderer->renderer_chunks_to_update, &chunk_renderer);
        }
    }
}

void renderer_world_destroy(renderer_world_t *renderer) {
    /*glDeleteLists(renderer->base_list_id, 4096 << 6 << 1);
    glDeleteLists(renderer->list_id, 2);
    array_list_free(renderer->chunks);
    free(renderer->chunk_data_cache);
    if(renderer->chunk_cache != NULL) {
        for(int i = 0; i < renderer->x_chunks; i++) {
            for(int j = 0; j < renderer->y_chunks; j++) {
                for(int k = 0; k < renderer->z_chunks; k++) {
                    int c = (k * renderer->y_chunks + j) * renderer->x_chunks + i;
                    free(renderer->chunk_cache[c]);
                }
            }
        }
        free(renderer->chunk_cache);
    }
    if(renderer->load_queue != NULL) free(renderer->load_queue);*/
}