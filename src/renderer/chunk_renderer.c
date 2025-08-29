#include <renderer/chunk_renderer.h>

#include <entity/entity.h>
#include <renderer/tesselator.h>
#include <world/block/blocks.h>
#include <world/chunk/chunk.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

int chunk_updates = 0;

void chunk_renderer_create(chunk_renderer_t *renderer, world_t *world, int x, int y, int z, int size, int render_list) {
    *renderer = (chunk_renderer_t){ 0 };
    renderer->world = world;
    renderer->render_list = render_list;
    renderer->width = size;
    renderer->height = size;
    renderer->depth = size;
    block_renderer_create(&renderer->block_renderer, world);
    chunk_renderer_set_position(renderer, x, y, z);
    renderer->is_visible = 1;
}

void chunk_renderer_set_position(chunk_renderer_t *renderer, int x, int y, int z) {
    if(x == renderer->x && y == renderer->y && z == renderer->z) return;
    chunk_renderer_dont_draw(renderer);
    renderer->x = x;
    renderer->y = y;
    renderer->z = z;
    renderer->x_plus = x + renderer->width / 2;
    renderer->y_plus = y + renderer->height / 2;
    renderer->z_plus = z + renderer->depth / 2;
    renderer->x_clip = x & 511;
    renderer->y_clip = y & 511;
    renderer->z_clip = z & 511;
    renderer->x_minus = x - renderer->x_clip;
    renderer->y_minus = y - renderer->y_clip;
    renderer->z_minus = z - renderer->z_clip;
    renderer->bb = (AABB_t){
        .x0 = x,
        .y0 = y,
        .z0 = z,
        .x1 = x + renderer->width,
        .y1 = y + renderer->height,
        .z1 = z + renderer->depth
    };
    renderer->bb = AABB_grow(renderer->bb, 2.0, 2.0, 2.0);
    glNewList(renderer->render_list + 2, GL_COMPILE);
    AABB_t clip_bb = (AABB_t){
        .x0 = renderer->x_clip - 2.0,
        .y0 = renderer->y_clip - 2.0,
        .z0 = renderer->z_clip - 2.0,
        .x1 = renderer->x_clip + 2.0 + renderer->width,
        .y1 = renderer->y_clip + 2.0 + renderer->height,
        .z1 = renderer->z_clip + 2.0 + renderer->depth
    };

    tesselator_begin_quads();

    tesselator_vertex(clip_bb.x0, clip_bb.y1, clip_bb.z0);
    tesselator_vertex(clip_bb.x1, clip_bb.y1, clip_bb.z0);
    tesselator_vertex(clip_bb.x1, clip_bb.y0, clip_bb.z0);
    tesselator_vertex(clip_bb.x0, clip_bb.y0, clip_bb.z0);

    tesselator_vertex(clip_bb.x0, clip_bb.y0, clip_bb.z1);
    tesselator_vertex(clip_bb.x1, clip_bb.y0, clip_bb.z1);
    tesselator_vertex(clip_bb.x1, clip_bb.y1, clip_bb.z1);
    tesselator_vertex(clip_bb.x0, clip_bb.y1, clip_bb.z1);

    tesselator_vertex(clip_bb.x0, clip_bb.y0, clip_bb.z0);
    tesselator_vertex(clip_bb.x1, clip_bb.y0, clip_bb.z0);
    tesselator_vertex(clip_bb.x1, clip_bb.y0, clip_bb.z1);
    tesselator_vertex(clip_bb.x0, clip_bb.y0, clip_bb.z1);

    tesselator_vertex(clip_bb.x0, clip_bb.y1, clip_bb.z1);
    tesselator_vertex(clip_bb.x1, clip_bb.y1, clip_bb.z1);
    tesselator_vertex(clip_bb.x1, clip_bb.y1, clip_bb.z0);
    tesselator_vertex(clip_bb.x0, clip_bb.y1, clip_bb.z0);

    tesselator_vertex(clip_bb.x0, clip_bb.y0, clip_bb.z1);
    tesselator_vertex(clip_bb.x0, clip_bb.y1, clip_bb.z1);
    tesselator_vertex(clip_bb.x0, clip_bb.y1, clip_bb.z0);
    tesselator_vertex(clip_bb.x0, clip_bb.y0, clip_bb.z0);

    tesselator_vertex(clip_bb.x1, clip_bb.y0, clip_bb.z0);
    tesselator_vertex(clip_bb.x1, clip_bb.y1, clip_bb.z0);
    tesselator_vertex(clip_bb.x1, clip_bb.y1, clip_bb.z1);
    tesselator_vertex(clip_bb.x1, clip_bb.y0, clip_bb.z1);

    tesselator_end();
    glEndList();
    renderer->needs_update = 1;
}

void chunk_renderer_update(chunk_renderer_t *renderer) {
    if(!renderer->needs_update) return;
    chunk_updates++;
    int x0 = renderer->x;
    int y0 = renderer->y;
    int z0 = renderer->z;
    int x1 = renderer->x + renderer->width;
    int y1 = renderer->y + renderer->height;
    int z1 = renderer->z + renderer->depth;

    chunk_is_lit = 0;
    for(int i = 0; i < 2; i++) {
        renderer->skip_render[i] = 1;
    }
    renderer->is_lit = 0;
    for(int i = 0; i < 2; i++) {
        uint8_t b0 = 0;
        uint8_t b1 = 0;

        glNewList(renderer->render_list + i, GL_COMPILE);
        glPushMatrix();
        glTranslatef(renderer->x_clip, renderer->y_clip, renderer->z_clip);
        tesselator_begin_quads();
        tesselator_set_translation(-renderer->x_clip, -renderer->y_clip, -renderer->z_clip);

        for(int x = x0; x < x1; x++) {
            for(int y = y0; y < y1; y++) {
                for(int z = z0; z < z1; z++) {
                    uint8_t block_id = world_get_block(renderer->world, x, y, z);
                    if(block_id > 0) {
                        block_t *block = &block_list[block_id];
                        if(block->render_pass != i) b0 = 1;
                        else b1 |= block->render(block, renderer->world, x, y, z);
                    }
                }
            }
        }

        tesselator_end();
        glPopMatrix();
        glEndList();
        tesselator_set_translation(0, 0, 0);
        if(b1) renderer->skip_render[i] = 0;
        if(!b0) break;
    }

    renderer->is_lit = chunk_is_lit;
}

float chunk_renderer_distance_to_entity_squared(chunk_renderer_t *renderer, entity_t *entity) {
    float dx = entity->x - renderer->x;
    float dy = entity->y - renderer->y;
    float dz = entity->z - renderer->z;
    return dx * dx + dy * dy + dz * dz;
}

void chunk_renderer_dont_draw(chunk_renderer_t *renderer) {
    for(int i = 0; i < 2; i++) {
        renderer->skip_render[i] = 1;
    }
}

void chunk_renderer_stop_rendering(chunk_renderer_t *renderer) {
    chunk_renderer_dont_draw(renderer);
    renderer->world = NULL;
}

int chunk_renderer_get_gl_call_list(chunk_renderer_t *renderer, int list) {
    return !renderer->is_in_frustum ? -1 : (!renderer->skip_render[list] ? renderer->render_list + list : -1);
}

void chunk_renderer_update_frustum(chunk_renderer_t *renderer, frustum_t *frustum) {
    renderer->is_in_frustum = frustum_contains_box(*frustum, renderer->bb.x0, renderer->bb.y0, renderer->bb.z0, renderer->bb.x1, renderer->bb.y1, renderer->bb.z1);
}

void chunk_renderer_call_occlusion_query_list(chunk_renderer_t *renderer) {
    glCallList(renderer->render_list + 2);
}

uint8_t chunk_renderer_skip_all_render_passes(chunk_renderer_t *renderer) {
    return renderer->skip_render[0] && renderer->skip_render[1];
}

int chunk_renderer_compare(const void *a, const void *b) {
    chunk_renderer_t *chunk_a = *(chunk_renderer_t **)a;
    chunk_renderer_t *chunk_b = *(chunk_renderer_t **)b;
    return chunk_renderer_distance_to_entity_squared(chunk_a, chunk_a->world->player) < chunk_renderer_distance_to_entity_squared(chunk_b, chunk_b->world->player) ? -1 : 1;
}