#include <renderer/renderer_screen_overlay.h>

#include <item/item.h>
#include <renderer/renderer_lighting.h>
#include <renderer/tesselator.h>
#include <minecraft.h>
#include <world/block/blocks.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include <string.h>

// this file deals with the item in hand, fire, and water effects

renderer_screen_overlay_t renderer_screen_overlay_create(world_t *world) {
    renderer_screen_overlay_t renderer = { 0 };
    memset(&renderer, 0, sizeof(renderer_screen_overlay_t));

    renderer.world = world;
    renderer.renderer_block = renderer_block_create(world);

    return renderer;
}

void renderer_screen_overlay_render(renderer_screen_overlay_t *renderer, float partial_tick) {
    float d_equipped = renderer->equipped_progress_old + (renderer->equipped_progress - renderer->equipped_progress_old) * partial_tick;
    entity_t *player = renderer->world->player;
    glPushMatrix();
    glRotatef(player->y_roto + (player->y_rot - player->y_roto) * partial_tick, 1.0, 0.0, 0.0);
    glRotatef(player->x_roto + (player->x_rot - player->x_roto) * partial_tick, 0.0, 1.0, 0.0);
    renderer_lighting_enable();
    glPopMatrix();
    float brightness = world_get_brightness(player->world, floor_double(player->x), floor_double(player->y), floor_double(player->z));
    glColor4f(brightness, brightness, brightness, 1.0);
    if(renderer->item_stack.item_id != 0) {
        // render block in hand
        glPushMatrix();
        if(renderer->is_swinging) {
            float t = (renderer->swing_time + partial_tick) / 8.0;
            float st = tsin(t * M_PI);
            float sst = tsin(sqrtf(t) * M_PI);
            glTranslatef(-sst * 0.4, tsin(sqrtf(t) * M_PI * 2.0) * 0.2, -st * 0.2);
        }

        glTranslatef(0.56, -0.52 - (1.0 - d_equipped) * 0.6, -0.72);
        glRotatef(45, 0, 1, 0);
        glEnable(GL_NORMALIZE);

        if(renderer->is_swinging) {
            float t = (renderer->swing_time + partial_tick) / 8.0;
            float st = tsin(t * t * M_PI);
            float sst = tsin(sqrtf(t) * M_PI);
            glRotatef(-st * 20.0, 0, 1, 0);
            glRotatef(-sst * 20.0, 0, 0, 1);
            glRotatef(-sst * 80.0, 1, 0, 0);
        }

        glScalef(0.4, 0.4, 0.4);

        if(renderer->item_stack.item_id < 256 && block_list[renderer->item_stack.item_id].render_type == BLOCK_RENDER_TYPE_NORMAL) {
            glBindTexture(GL_TEXTURE_2D, textures_load(&renderer->world->minecraft->textures, "terrain.png"));
            renderer_block_render_in_inventory(&renderer->renderer_block, &block_list[renderer->item_stack.item_id]);
        }else {
            if(renderer->item_stack.item_id < 256) {
                glBindTexture(GL_TEXTURE_2D, textures_load(&renderer->world->minecraft->textures, "terrain.png"));
            }else {
                glBindTexture(GL_TEXTURE_2D, textures_load(&renderer->world->minecraft->textures, "gui/items.png"));
            }

            item_stack_t item = renderer->item_stack;
            float u0 = (item_list[item.item_id].texture_id % 16 << 4) / 256.0;
            float u1 = ((item_list[item.item_id].texture_id % 16 << 4) + 16) / 256.0;
            float v0 = (item_list[item.item_id].texture_id / 16 << 4) / 256.0;
            float v1 = ((item_list[item.item_id].texture_id / 16 << 4) + 16) / 256.0;

            glEnable(GL_NORMALIZE);
            glTranslatef(0, -0.3, 0);
            glScalef(1.5, 1.5, 1.5);
            glRotatef(50, 0, 1, 0);
            glRotatef(335, 0, 0, 1);
            glTranslatef(-(15.0 / 16.0), -(1.0 / 16.0), 0);

            // front face
            tesselator_normal(0, 0, 1);
            tesselator_begin_quads();
            tesselator_vertex_uv(0, 0, 0, u1, v1);
            tesselator_vertex_uv(1, 0, 0, u0, v1);
            tesselator_vertex_uv(1, 1, 0, u0, v0);
            tesselator_vertex_uv(0, 1, 0, u1, v0);
            tesselator_end();
            
            // back face
            tesselator_normal(0, 0, -1);
            tesselator_begin_quads();
            tesselator_vertex_uv(0, 1, -0.0625, u1, v0);
            tesselator_vertex_uv(1, 1, -0.0625, u0, v0);
            tesselator_vertex_uv(1, 0, -0.0625, u0, v1);
            tesselator_vertex_uv(0, 0, -0.0625, u1, v1);
            tesselator_end();

            // left face
            tesselator_normal(-1, 0, 0);
            tesselator_begin_quads();
            for(int i = 0; i < 16; i++) {
                float fx = i / 16.0;
                float u = u1 + (u0 - u1) * fx - 0.001953125;
                float x = fx * 1.0;
                tesselator_vertex_uv(x, 0, -0.0625, u, v1);
                tesselator_vertex_uv(x, 0, 0, u, v1);
                tesselator_vertex_uv(x, 1, 0, u, v0);
                tesselator_vertex_uv(x, 1, -0.0625, u, v0);
            }
            tesselator_end();

            // right face
            tesselator_normal(1, 0, 0);
            tesselator_begin_quads();
            for(int i = 0; i < 16; i++) {
                float fx = i / 16.0;
                float u = u1 + (u0 - u1) * fx - 0.001953125;
                float x = fx + 1.0 / 16.0;
                tesselator_vertex_uv(x, 1, -0.0625, u, v0);
                tesselator_vertex_uv(x, 1, 0, u, v0);
                tesselator_vertex_uv(x, 0, 0, u, v1);
                tesselator_vertex_uv(x, 0, -0.0625, u, v1);
            }
            tesselator_end();

            // top face
            tesselator_normal(0, 1, 0);
            tesselator_begin_quads();
            for(int i = 0; i < 16; i++) {
                float fy = i / 16.0;
                float v = v1 + (v0 - v1) * fy - 0.001953125;
                float y = fy + 1.0 / 16.0;
                tesselator_vertex_uv(0, y, 0, u1, v);
                tesselator_vertex_uv(1, y, -0.0625, u0, v);
                tesselator_vertex_uv(1, y, -0.0625, u0, v);
                tesselator_vertex_uv(0, y, 0, u1, v);
            }
            tesselator_end();

            // bottom face
            tesselator_normal(0, -1, 0);
            tesselator_begin_quads();
            for(int i = 0; i < 16; i++) {
                float fy = i / 16.0;
                float v = v1 + (v0 - v1) * fy - 0.001953125;
                float y = fy * 1.0;
                tesselator_vertex_uv(1, y, 0, u0, v);
                tesselator_vertex_uv(0, y, 0, u1, v);
                tesselator_vertex_uv(0, y, -0.0625, u1, v);
                tesselator_vertex_uv(1, y, -0.0625, u0, v);
            }
            tesselator_end();

            glDisable(GL_NORMALIZE);
        }

        glPopMatrix();
    }else {
        // render just the hand
        glPushMatrix();
        if(renderer->is_swinging) {
            float t = (renderer->swing_time + partial_tick) / 8.0;
            float st = tsin(t * M_PI);
            float sst = tsin(sqrtf(t) * M_PI);
            glTranslatef(-sst * 0.3, tsin(sqrtf(t) * M_PI * 2.0) * 0.4, -st * 0.4);
        }

        glTranslatef(0.64, -0.6 - (1.0 - d_equipped) * 0.6, -0.72);
        glRotatef(45, 0, 1, 0);
        glEnable(GL_NORMALIZE);
        if(renderer->is_swinging) {
            float t = (renderer->swing_time + partial_tick) / 8.0;
            float st = tsin(t * t * M_PI);
            float sst = tsin(sqrtf(t) * M_PI);
            glRotatef(sst * 70.0, 0, 1, 0);
            glRotatef(-st * 20.0, 0, 0, 1);
        }

        // TODO: grab skin data
        glBindTexture(GL_TEXTURE_2D, textures_load(&renderer->world->minecraft->textures, "char.png"));
        glTranslatef(-0.2, -0.3, 0.1);
        glRotatef(120, 0, 0, 1);
        glRotatef(200, 1, 0, 0);
        glRotatef(-135, 0, 1, 0);
        glScalef(1.0 / 16.0, 1.0 / 16.0, 1.0 / 16.0);
        glTranslatef(6, 0, 0);
        // render player draw first person hand
        // "temporary" fix here:
        cube_render(&models_get(&renderer->world->minecraft->models, MODEL_HUMANOID)->arm0, 1.0);
        glPopMatrix();
    }
    
    glDisable(GL_NORMALIZE);
    renderer_lighting_disable();
}

void renderer_screen_overlay_render_overlays(renderer_screen_overlay_t *renderer, float partial_tick) {
    glDisable(GL_ALPHA_TEST);
    if(renderer->world->player->fire > 0) {
        glBindTexture(GL_TEXTURE_2D, textures_load(&renderer->world->minecraft->textures, "terrain.png"));
        glColor4f(1, 1, 1, 0.9);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for(int i = 0; i < 2; i++) {
            glPushMatrix();
            int texture = blocks.fire.texture_id + (i << 4);
            int u = (texture & 15) << 4;
            texture &= 240;
            float u0 = u / 256.0;
            float u1 = (u + 15.99) / 256.0;
            float v0 = texture / 256.0;
            float v1 = (texture + 15.99) / 256.0;

            glTranslatef((-((i << 1) - 1)) * 0.24, -0.3, 0);
            glRotatef(((i << 1) - 1) * 10.0, 0, 1, 0);

            tesselator_begin_quads();
            tesselator_vertex_uv(-0.5, -0.5, -0.5, u1, v1);
            tesselator_vertex_uv(0.5, -0.5, -0.5, u0, v1);
            tesselator_vertex_uv(0.5, 0.5, -0.5, u0, v0);
            tesselator_vertex_uv(-0.5, 0.5, -0.5, u1, v0);
            tesselator_end();

            glPopMatrix();
        }

        glColor4f(1, 1, 1, 1);
        glDisable(GL_BLEND);
    }

    if(entity_is_underwater(renderer->world->player)) {
        glBindTexture(GL_TEXTURE_2D, textures_load(&renderer->world->minecraft->textures, "water.png"));
        float brightness = entity_get_brightness(renderer->world->player, partial_tick);
        glColor4f(brightness, brightness, brightness, 0.5);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPushMatrix();
        float rot_x = -renderer->world->player->x_rot / 64.0;
        float rot_y = renderer->world->player->y_rot / 64.0;
        tesselator_begin_quads();
        tesselator_vertex_uv(-1, -1, -0.5, rot_x + 4, rot_y + 4);
        tesselator_vertex_uv(1, -1, -0.5, rot_x, rot_y + 4);
        tesselator_vertex_uv(1, 1, -0.5, rot_x, rot_y);
        tesselator_vertex_uv(-1, 1, -0.5, rot_x + 4, rot_y);
        tesselator_end();
        glPopMatrix();
        glColor4f(1, 1, 1, 1);
        glDisable(GL_BLEND);
    }

    glEnable(GL_ALPHA_TEST);
}

void renderer_screen_overlay_update_item(renderer_screen_overlay_t *renderer) {
    renderer->equipped_progress_old = renderer->equipped_progress;
    if(renderer->is_swinging) {
        renderer->swing_time++;
        if(renderer->swing_time == 8) {
            renderer->swing_time = 0;
            renderer->is_swinging = 0;
        }
    }

    entity_t *player = renderer->world->player;
    item_stack_t item = inventory_player_get_selected(&player->mob->player->inventory);
    float shift = item.item_id == renderer->item_stack.item_id ? 1 : 0;
    shift -= renderer->equipped_progress;
    if(shift < -0.4) {
        shift = -0.4;
    }
    if(shift > 0.4) {
        shift = 0.4;
    }

    renderer->equipped_progress += shift;
    if(renderer->equipped_progress < 0.1) {
        renderer->item_stack = item;
    }
}