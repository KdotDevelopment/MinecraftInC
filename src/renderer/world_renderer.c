#include <renderer/level_renderer.h>
#include <renderer/tesselator.h>

#include <util/array_list.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include <stdio.h>

level_renderer_t level_renderer_create(struct minecraft_s *minecraft, level_t *level, textures_t *textures) {
    level_renderer_t renderer = { 0 };
    renderer.chunks = array_list_create(sizeof(chunk_t *));
    renderer.chunk_data_cache = calloc(65536, sizeof(int));
    renderer.ticks = 0;
    renderer.last_load_x = -9999;
    renderer.last_load_y = -9999;
    renderer.last_load_z = -9999;
    renderer.textures = textures;
    renderer.list_id = glGenLists(2);
    renderer.base_list_id = glGenLists(4096 << 6 << 1);
    renderer.level = level;

    return renderer;
}

void level_renderer_refresh(level_renderer_t *renderer) {
    if(renderer->chunk_cache != NULL) {
        for(int i = 0; i < renderer->chunk_cache_count; i++) chunk_dispose(renderer->chunk_cache[i]);
        free(renderer->chunk_cache);
        free(renderer->load_queue);
    }

    renderer->x_chunks = (int)renderer->level->width / 16;
    renderer->y_chunks = (int)renderer->level->depth / 16;
    renderer->z_chunks = (int)renderer->level->height / 16;
    renderer->chunk_cache_count = renderer->x_chunks * renderer->y_chunks * renderer->z_chunks;
    renderer->chunk_cache = malloc(renderer->chunk_cache_count * sizeof(chunk_t *));
    renderer->load_queue = malloc(renderer->chunk_cache_count * sizeof(chunk_t *));

    int l = 0;
    for(int i = 0; i < renderer->x_chunks; i++) {
        for(int j = 0; j < renderer->y_chunks; j++) {
            for(int k = 0; k < renderer->z_chunks; k++) {
                int c = (k * renderer->y_chunks + j) * renderer->x_chunks + i;
                renderer->chunk_cache[c] = malloc(sizeof(chunk_t));
                *renderer->chunk_cache[c] = chunk_create(renderer->level, i << 4, j << 4, k << 4, 16, renderer->base_list_id + l);
                renderer->load_queue[c] = renderer->chunk_cache[c];
                l += 2;
            }
        }
    }

    for(int i = 0; i < array_list_length(renderer->chunks); i++) {
        renderer->chunks[i]->loaded = 0;
    }
    renderer->chunks = array_list_clear(renderer->chunks);

    glNewList(renderer->list_id, GL_COMPILE);
    glColor4f(0.5, 0.5, 0.5, 1.0);
    float ground = level_get_ground_level(renderer->level);
    int a = fmin(128, fmin(renderer->level->width, renderer->level->height));
    int b = 2048 / a;
    tesselator_begin();
    for(int i = -a * b; i < (int)renderer->level->width + a * b; i += a) {
        for(int j = -a * b; j < (int)renderer->level->height + a * b; j += a) {
            float g = ground;
            if(i >= 0 && j >= 0 && i < (int)renderer->level->width && j < renderer->level->height) {
                g = 0;
            }
            tesselator_vertex_uv(i, g, j + a, 0.0, a);
            tesselator_vertex_uv(i + a, g, j + a, a, a);
            tesselator_vertex_uv(i + a, g, j, a, 0.0);
            tesselator_vertex_uv(i, g, j, 0.0, 0.0);
        }
    }
    tesselator_end();

    glColor3f(0.8, 0.8, 0.8);
    tesselator_begin();
    for(int i = 0; i < (int)renderer->level->width; i += a) {
        tesselator_vertex_uv(i, 0.0, 0.0, 0.0, 0.0);
        tesselator_vertex_uv(i + a, 0.0, 0.0, a, 0.0);
        tesselator_vertex_uv(i + a, ground, 0.0, a, ground);
        tesselator_vertex_uv(i, ground, 0.0, 0.0, ground);
        tesselator_vertex_uv(i, ground, renderer->level->height, 0.0, ground);
        tesselator_vertex_uv(i + a, ground, renderer->level->height, a, ground);
        tesselator_vertex_uv(i + a, 0.0, renderer->level->height, a, 0.0);
        tesselator_vertex_uv(i, 0.0, renderer->level->height, 0.0, 0.0);
    }
    glColor3f(0.6, 0.6, 0.6);
    for(int i = 0; i < (int)renderer->level->height; i += a) {
        tesselator_vertex_uv(0.0, ground, i, 0.0, 0.0);
        tesselator_vertex_uv(0.0, ground, i + a, a, 0.0);
        tesselator_vertex_uv(0.0, 0.0, i + a, a, ground);
        tesselator_vertex_uv(0.0, 0.0, i, 0.0, ground);
        tesselator_vertex_uv(renderer->level->width, 0.0, i, 0.0, ground);
        tesselator_vertex_uv(renderer->level->width, 0.0, i + a, a, ground);
        tesselator_vertex_uv(renderer->level->width, ground, i + a, a, 0.0);
        tesselator_vertex_uv(renderer->level->width, ground, i, 0.0, 0.0);
    }
    tesselator_end();
    glEndList();

    glNewList(renderer->list_id + 1, GL_COMPILE);
    glColor3f(1.0, 1.0, 1.0);
    float water = level_get_water_level(renderer->level);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    tesselator_begin();
    for(int i = -a * b; i < (int)renderer->level->width + a * b; i += a) {
        for(int j = -a * b; j < (int)renderer->level->height + a * b; j += a) {
            float w = water - 0.1;
            if(i < 0 || j < 0 || i >= (int)renderer->level->width || j >= (int)renderer->level->height) {
                tesselator_vertex_uv(i, w, j + a, 0.0, a);
                tesselator_vertex_uv(i + a, w, j + a, a, a);
                tesselator_vertex_uv(i + a, w, j, a, 0.0);
                tesselator_vertex_uv(i, w, j, 0.0, 0.0);
                tesselator_vertex_uv(i, w, j, 0.0, 0.0);
                tesselator_vertex_uv(i + a, w, j, a, 0.0);
                tesselator_vertex_uv(i + a, w, j + a, a, a);
                tesselator_vertex_uv(i, w, j + a, 0.0, a);
            }
        }
    }
    tesselator_end();
    glDisable(GL_BLEND);
    glEndList();
    level_renderer_queue_chunks(renderer, 0, 0, 0, renderer->level->width, renderer->level->depth, renderer->level->height);
}

int level_renderer_sort_chunks(level_renderer_t *renderer, player_t *player, int pass) {
    float vx = player->x - renderer->last_load_x;
    float vy = player->y - renderer->last_load_y;
    float vz = player->z - renderer->last_load_z;
    if(vx * vx + vy * vy + vz * vz > 64.0) {
        renderer->last_load_x = player->x;
        renderer->last_load_y = player->y;
        renderer->last_load_z = player->z;
        qsort(renderer->load_queue, renderer->chunk_cache_count, sizeof(chunk_t *), chunk_list_comparator);
    }
    int count = 0;
    for(int i = 0; i < renderer->chunk_cache_count; i++) {
        count = chunk_append_lists(renderer->load_queue[i], renderer->chunk_data_cache, count, pass);
    }
    if(count > 0) {
        glBindTexture(GL_TEXTURE_2D, textures_load(renderer->textures, "terrain.png"));
        glCallLists(count, GL_INT, renderer->chunk_data_cache);
    }

    return count;
}

void level_renderer_queue_chunks(level_renderer_t *renderer, int x0, int y0, int z0, int x1, int y1, int z1) {
    x0 /= 16;
    y0 /= 16;
    z0 /= 16;
    x1 /= 16;
    y1 /= 16;
    z1 /= 16;
    x0 = x0 < 0 ? 0 : x0;
    y0 = y0 < 0 ? 0 : y0;
    z0 = z0 < 0 ? 0 : z0;
    x1 = x1 > renderer->x_chunks - 1 ? renderer->x_chunks - 1 : x1;
    y1 = y1 > renderer->y_chunks - 1 ? renderer->y_chunks - 1 : y1;
    z1 = z1 > renderer->z_chunks - 1 ? renderer->z_chunks - 1 : z1;
    for (int x = x0; x <= x1; x++) {
        for (int y = y0; y <= y1; y++) {
            for (int z = z0; z <= z1; z++) {
                chunk_t *chunk = renderer->chunk_cache[(z * renderer->y_chunks + y) * renderer->x_chunks + x];
                if(!chunk->loaded) {
                    chunk->loaded = 1;
                    renderer->chunks = array_list_push(renderer->chunks, &chunk);
                }
            }
        }
    }
}

void level_renderer_destroy(level_renderer_t *renderer) {
    glDeleteLists(renderer->base_list_id, 4096 << 6 << 1);
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
    if(renderer->load_queue != NULL) free(renderer->load_queue);
}