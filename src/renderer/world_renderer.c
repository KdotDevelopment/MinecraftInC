#include <renderer/world_renderer.h>

#include <renderer/tesselator.h>
#include <world/block/blocks.h>
#include <minecraft.h>

#include <util/array_list.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include <stdio.h>

world_renderer_t world_renderer_create(struct minecraft_s *minecraft, world_t *world, textures_t *textures) {
    world_renderer_t renderer = { 0 };

    renderer.minecraft = minecraft;
    renderer.ticks = 0;
    renderer.last_load_x = -9999;
    renderer.last_load_y = -9999;
    renderer.last_load_z = -9999;
    renderer.textures = textures;
    renderer.list_id = glGenLists(2);
    renderer.render_list_base = glGenLists(786432);
    renderer.world = world;
    renderer.chunk_renderers_to_update = array_list_create(sizeof(uint64_t));

    if(renderer.occlusion_enabled) {
        memset(renderer.occlusion_query_base, 0, sizeof(renderer.occlusion_query_base));
        glGenQueriesARB(262144, renderer.occlusion_query_base);
    }

    // Star renderer
    renderer.star_render_list = glGenLists(1);
    glNewList(renderer.star_render_list, GL_COMPILE);
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
    renderer.sky_render_list = glGenLists(1);
    glNewList(renderer.sky_render_list, GL_COMPILE);
    tesselator_begin_quads();

    for(int x = 0; x <= 256; x += 32) {
        for(int y = 0; y <= 256; y += 32) {
            tesselator_vertex(x, 16.0, y);
            tesselator_vertex(x + 32.0, 16.0, y);
            tesselator_vertex(x + 32.0, 16.0, y + 32.0);
            tesselator_vertex(x, 16.0, y + 32.0);
        }
    }

    tesselator_end();
    glEndList();

    return renderer;
}

void world_renderer_change_world(world_renderer_t *renderer, world_t *world) {
    if(renderer->world != NULL) {
        world_remove_renderer(renderer->world, renderer);
    }

    renderer->last_load_x = -9999;
    renderer->last_load_y = -9999;
    renderer->last_load_z = -9999;
    // rendermanager set world
    block_renderer_create(&renderer->block_renderer, world);
    if(world != NULL) {
        world_add_renderer(world, renderer);
        world_renderer_load_renderers(renderer);
    }
}

void world_renderer_load_renderers(world_renderer_t *renderer) {
    renderer->render_distance = renderer->minecraft->settings.view_distance;
    if(renderer->chunk_renderers != NULL) {
        for(int i = 0; i < renderer->chunk_renderer_count; i++) {
            chunk_renderer_t *chunk_renderer = *(chunk_renderer_t **)renderer->chunk_renderers[i];
            chunk_renderer_stop_rendering(chunk_renderer);
            free(chunk_renderer);
        }
    }

    free(renderer->chunk_renderers);
    free(renderer->chunk_renderers_sorted);

    int distance = 5 << 3 - renderer->render_distance;
    if(distance > 28) {
        distance = 28;
    }

    renderer->x_chunks = distance;
    renderer->y_chunks = 8;
    renderer->z_chunks = distance;
    renderer->chunk_renderers = malloc(renderer->x_chunks * renderer->y_chunks * renderer->z_chunks * sizeof(uint64_t));
    renderer->chunk_renderers_sorted = malloc(renderer->x_chunks * renderer->y_chunks * renderer->z_chunks * sizeof(uint64_t));
    renderer->chunk_renderer_count = renderer->x_chunks * renderer->y_chunks * renderer->z_chunks;
    renderer->x0 = 0;
    renderer->y0 = 0;
    renderer->z0 = 0;
    renderer->x1 = renderer->x_chunks;
    renderer->y1 = renderer->y_chunks;
    renderer->z1 = renderer->z_chunks;

    for(int i = 0; i < array_list_length(renderer->chunk_renderers_to_update); i++) {
        chunk_renderer_t *chunk_renderer = *(chunk_renderer_t **)array_list_get(renderer->chunk_renderers_to_update, i);
        chunk_renderer->needs_update = 0;
    }

    renderer->chunk_renderers_to_update = array_list_clear(renderer->chunk_renderers_to_update);

    int render_list_index = 0;
    int chunk_index = 0;

    for(int x = 0; x < renderer->x_chunks; x++) {
        for(int y = 0; y < renderer->y_chunks; y++) {
            for(int z = 0; z < renderer->z_chunks; z++) {
                chunk_renderer_t *chunk_renderer = malloc(sizeof(chunk_renderer_t));
                chunk_renderer_create(chunk_renderer, renderer->world, x * CHUNK_SIZE_WIDTH, y * CHUNK_SIZE_WIDTH, z * CHUNK_SIZE_WIDTH, CHUNK_SIZE_WIDTH, renderer->render_list_base + render_list_index);
                renderer->chunk_renderers[(z * renderer->y_chunks + y) * renderer->x_chunks + x] = chunk_renderer;
                if(renderer->occlusion_enabled) {
                    chunk_renderer->gl_occlusion_query = renderer->occlusion_query_base[chunk_index];
                }
                chunk_renderer->waiting_on_occlusion = 0;
                chunk_renderer->is_visible = 1;
                chunk_renderer->is_in_frustum = 1;
                chunk_index++;
                chunk_renderer->needs_update = 1;
                renderer->chunk_renderers_sorted[(z * renderer->y_chunks + y) * renderer->x_chunks + x] = renderer->chunk_renderers[(z * renderer->y_chunks + y) * renderer->x_chunks + x];
                renderer->chunk_renderers_to_update = array_list_push(renderer->chunk_renderers_to_update, &renderer->chunk_renderers[(z * renderer->y_chunks + y) * renderer->x_chunks + x]);
                render_list_index += 3;
            }
        }
    }

    entity_t *player = &renderer->minecraft->player.mob.entity;
    world_renderer_new_position(renderer, floor_double(player->x), floor_double(player->y), floor_double(player->z));
    qsort(renderer->chunk_renderers_sorted, renderer->chunk_renderer_count, sizeof(chunk_renderer_t *), chunk_renderer_compare);
}

void world_renderer_update_entities(world_renderer_t *renderer, vec3_t pos, frustum_t *frustum, float partial_tick) {
    // render_manager cache active render info
    renderer->entities_total = 0;
    renderer->entities_rendered = 0;
    renderer->entities_hidden = 0;
    entity_t *player = &renderer->world->player->mob.entity;
    // render_manager.x = renderer->minecraft->player.xo + (renderer->minecraft->player.x - renderer->minecraft->player.xo) * partial_tick;
    // render_manager.y = renderer->minecraft->player.yo + (renderer->minecraft->player.y - renderer->minecraft->player.yo) * partial_tick;
    // render_manager.z = renderer->minecraft->player.zo + (renderer->minecraft->player.z - renderer->minecraft->player.zo) * partial_tick;
    entity_t **entities = array_list_clone(renderer->world->loaded_entity_list);
    renderer->entities_total = array_list_length(entities);

    for(int i = 0; i < renderer->entities_total; i++) {
        entity_t *entity = array_list_get(entities, i);
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
        
        if(distance_sq < avg_size && frustum_contains_box_bb(*frustum, bb) && (entity != &renderer->world->player->mob.entity || renderer->minecraft->settings.third_person)) {
            renderer->entities_rendered++;
            // render_manager render entity (entity, partial_tick)
        }
    }

    array_list_free(entities);
}

void world_renderer_update_clouds(world_renderer_t *renderer) {
    renderer->cloud_offset_x++;
}

void world_renderer_draw_sky(world_renderer_t *renderer, float partial_tick) {
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
    int cloud_r = cloud_color.x;
    int cloud_g = cloud_color.y;
    int cloud_b = cloud_color.z;
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

void world_renderer_update_renderers(world_renderer_t *renderer, entity_t *player) {

}

void world_renderer_draw_block_breaking(world_renderer_t *renderer, entity_t *player, hit_result_t *hit_result, int unused, item_stack_t *item_stack, float partial_tick) {
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
        double x = player->xo + (player->x - player->xo) * partial_tick;
        double y = player->yo + (player->y - player->yo) * partial_tick;
        double z = player->zo + (player->z - player->zo) * partial_tick;
        tesselator_set_translation(-x, -y, -z);
        tesselator_disable_color();
        if(block == NULL) block = &block_list[blocks.stone.id];
        //global block renderer render block using texture
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

void world_renderer_draw_selection_box(world_renderer_t *renderer, entity_t *player, hit_result_t *hit_result, uint8_t block_id, float partial_tick) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0, 0.0, 0.0, 0.4);
    glLineWidth(2.0);
    glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_FALSE);
    uint8_t block_id = world_get_block(renderer->world, hit_result->x, hit_result->y, hit_result->z);
    block_t *block = &block_list[block_id];
    if(block_id > 0) {
        double x = player->xo + (player->x - player->xo) * partial_tick;
        double y = player->yo + (player->y - player->yo) * partial_tick;
        double z = player->zo + (player->z - player->zo) * partial_tick;
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

void world_renderer_queue_chunks(world_renderer_t *renderer, int x0, int y0, int z0, int x1, int y1, int z1) {
    x0 /= CHUNK_SIZE_WIDTH;
    y0 /= CHUNK_SIZE_WIDTH;
    z0 /= CHUNK_SIZE_WIDTH;
    x1 /= CHUNK_SIZE_WIDTH;
    y1 /= CHUNK_SIZE_WIDTH;
    z1 /= CHUNK_SIZE_WIDTH;
    
    for (int x = x0; x <= x1; x++) {
        int wrapped_x = x % renderer->x_chunks;
        if(wrapped_x < 0) wrapped_x += renderer->x_chunks;

        for (int y = y0; y <= y1; y++) {
            int wrapped_y = y % renderer->y_chunks;
            if(wrapped_y < 0) wrapped_y += renderer->y_chunks;

            for (int z = z0; z <= z1; z++) {
                int wrapped_z = z % renderer->z_chunks;
                if(wrapped_z < 0) wrapped_z += renderer->z_chunks;

                chunk_renderer_t *chunk_renderer = renderer->chunk_renderers[(wrapped_z * renderer->y_chunks + wrapped_y) * renderer->x_chunks + wrapped_x];
                if(!chunk_renderer->needs_update) {
                    chunk_renderer->needs_update = 1;
                    renderer->chunk_renderers_to_update = array_list_push(renderer->chunk_renderers_to_update, &chunk_renderer);
                }
            }
        }
    }
}

void world_renderer_update_block(world_renderer_t *renderer, int x, int y, int z) {
    world_renderer_queue_chunks(renderer, x - 1, y - 1, z - 1, x + 1, y + 1, z + 1);
}

void world_renderer_update_blocks(world_renderer_t *renderer, int x0, int y0, int z0, int x1, int y1, int z1) {
    world_renderer_queue_chunks(renderer, x0 - 1, y0 - 1, z0 - 1, x1 + 1, y1 + 1, z1 + 1);
}

void world_renderer_update_frustum(world_renderer_t *renderer, frustum_t *frustum) {
    for(int i = 0; i < renderer->chunk_cache_count; i++) {
        chunk_renderer_t *chunk_renderer = renderer->chunk_renderers[i];
        chunk_renderer_update_frustum(chunk_renderer, frustum);
    }
}

void world_renderer_play_sound(world_renderer_t *renderer, uint8_t sound, double x, double y, double z, float volume, float pitch) {

}

void world_renderer_spawn_particle(world_renderer_t *renderer, uint8_t particle_type, double x, double y, double z, double x_vel, double y_vel, double z_vel) {
    
}

void world_renderer_destroy(world_renderer_t *renderer) {
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