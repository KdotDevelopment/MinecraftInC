#include <renderer/entity/renderer_entity_item.h>

#include <item/item.h>
#include <minecraft.h>
#include <renderer/entity/renderer_entity_manager.h>
#include <renderer/tesselator.h>
#include <world/block/block.h>
#include <world/block/blocks.h>

#include <util/random.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

renderer_entity_t renderer_entity_item_create() {
    renderer_entity_t renderer = renderer_entity_create();

    renderer.shadow_size = 0.15;
    renderer.shadow_opaque = 12.0 / 16.0;

    renderer.render = renderer_entity_item_render;

    return renderer;
}

void renderer_entity_item_render_gui(minecraft_t *minecraft, item_stack_t *item, int x, int y) {
    if(item->item_id != 0) {
        if(item->item_id < 256 && block_list[item->item_id].render_type == BLOCK_RENDER_TYPE_NORMAL) {
            glBindTexture(GL_TEXTURE_2D, textures_load(&minecraft->textures, "terrain.png"));
            block_t *block = &block_list[item->item_id];
            glPushMatrix();
            glTranslatef(x - 2, y + 3, 0);
            glScalef(10, 10, 10);
            glTranslatef(1, 0.5, 8.0);
            glRotatef(210, 1, 0, 0);
            glRotatef(45, 0, 1, 0);
            glColor4f(1, 1, 1, 1);
            renderer_block_t renderer_block = renderer_block_create(minecraft->world);
            renderer_block_render_in_inventory(&renderer_block, block);
            glPopMatrix();
        }else {
            if(item_list[item->item_id].texture_id >= 0) {
                glDisable(GL_LIGHTING);
                if(item->item_id < 256) {
                    glBindTexture(GL_TEXTURE_2D, textures_load(&minecraft->textures, "terrain.png"));
                }else {
                    glBindTexture(GL_TEXTURE_2D, textures_load(&minecraft->textures, "gui/items.png"));
                }

                int texture = item_list[item->item_id].texture_id;
                int tex_x = (texture % 16) << 4;
                int tex_y = (texture / 16) << 4;

                tesselator_begin_quads();
                tesselator_vertex_uv(x, y + 16, 0, tex_x / 256.0, (tex_y + 16) / 256.0);
                tesselator_vertex_uv(x + 16, y + 16, 0, (tex_x + 16) / 256.0, (tex_y + 16) / 256.0);
                tesselator_vertex_uv(x + 16, y, 0, (tex_x + 16) / 256.0, tex_y / 256.0);
                tesselator_vertex_uv(x, y, 0, tex_x / 256.0, tex_y / 256.0);
                tesselator_end();

                glEnable(GL_LIGHTING);
            }
        }
    }
}

void private_renderer_entity_item_render_quad(int x0, int y0, int x1, int y1, int color) {
    tesselator_begin_quads();
    tesselator_color_opaque_int(color);
    tesselator_vertex(x0, y0, 0);
    tesselator_vertex(x0, y0 + y1, 0);
    tesselator_vertex(x0 + x1, y0 + y1, 0);
    tesselator_vertex(x0 + x1, y0, 0);
    tesselator_end();
}

void renderer_entity_item_render_overlay_gui(font_t *font, item_stack_t *item, int x, int y) {
    if(item->item_id != 0) {
        if(item->stack_size > 1) {
            glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);
            char number[4];
            sprintf(number, "%d", item->stack_size);
            font_render(font, number, x + 19 - 2 - font_get_width(font, number), y + 6 + 3, 0xffffffff);
            glEnable(GL_LIGHTING);
            glEnable(GL_DEPTH_TEST);
        }

        if(item->item_damage > 0) {
            int width = 13 - item->item_damage * 13 / item_stack_get_max_damage(item);
            int green = 255 - item->item_damage * 255 / item_stack_get_max_damage(item);
            glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_TEXTURE_2D);
            int color = (255 - green) << 16 | green << 8;
            int bgcolor = (255 - green) / 4 << 16 | 16128;
            private_renderer_entity_item_render_quad(x + 2, y + 13, 13, 2, 0);
            private_renderer_entity_item_render_quad(x + 2, y + 13, 12, 1, bgcolor);
            private_renderer_entity_item_render_quad(x + 2, y + 13, width, 1, color);
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_LIGHTING);
            glEnable(GL_DEPTH_TEST);
            glColor4f(1, 1, 1, 1);
        }
    }
}

void renderer_entity_item_render(renderer_entity_t *renderer, entity_t *entity, double x, double y, double z, float rot_x, float partial_tick) {
    random_t rand = random_create(187);
    item_stack_t item_stack = entity->item_stack;
    glPushMatrix();
    float bounce = tsin((entity->age + partial_tick) / 10.0 + entity->hover_start) * 0.1 + 0.1;
    float spin = ((entity->age + partial_tick) / 20.0 + entity->hover_start) * (180.0 / M_PI);
    uint8_t stacking = 1;
    if(item_stack.stack_size > 1) stacking = 2;
    if(item_stack.stack_size > 5) stacking = 3;
    if(item_stack.stack_size > 20) stacking = 4;

    glTranslatef(x, y + bounce, z);
    glEnable(GL_NORMALIZE);
    if(item_stack.item_id < 256 && block_list[item_stack.item_id].render_type == BLOCK_RENDER_TYPE_NORMAL) {
        glRotatef(spin, 0, 1, 0);
        glBindTexture(GL_TEXTURE_2D, textures_load(&renderer->manager->world->minecraft->textures, "terrain.png"));
        float scale = 0.25;
        if(!block_list[item_stack.item_id].render_normal_block && item_stack.item_id != blocks.slab.id) {
            scale = 0.5;
        }

        glScalef(scale, scale, scale);

        for(int i = 0; i < stacking; i++) {
            glPushMatrix();
            if(i > 0) {
                float xx = (random_next_uniform(&rand) * 2.0 - 1.0) * 0.2 / scale;
                float yy = (random_next_uniform(&rand) * 2.0 - 1.0) * 0.2 / scale;
                float zz = (random_next_uniform(&rand) * 2.0 - 1.0) * 0.2 / scale;
                glTranslatef(xx, yy, zz);
            }

            renderer_block_t renderer_block = renderer_block_create(renderer->manager->world);
            renderer_block_render_in_inventory(&renderer_block, &block_list[item_stack.item_id]);
            glPopMatrix();
        }
    }else {
        glScalef(0.5, 0.5, 0.5);
        int texture = item_list[item_stack.item_id].texture_id;
        if(item_stack.item_id < 256) {
            glBindTexture(GL_TEXTURE_2D, textures_load(&renderer->manager->world->minecraft->textures, "terrain.png"));
        }else {
            glBindTexture(GL_TEXTURE_2D, textures_load(&renderer->manager->world->minecraft->textures, "gui/items.png"));
        }

        float u0 = (texture % 16 << 4) / 256.0;
        float u1 = ((texture % 16 << 4) + 16) / 256.0;
        float v0 = (texture / 16 << 4) / 256.0;
        float v1 = ((texture / 16 << 4) + 16) / 256.0;

        for(int i = 0; i < stacking; i++) {
            float xx = (random_next_uniform(&rand) * 2.0 - 1.0) * 0.3;
            float yy = (random_next_uniform(&rand) * 2.0 - 1.0) * 0.3;
            float zz = (random_next_uniform(&rand) * 2.0 - 1.0) * 0.3;
            glTranslatef(xx, yy, zz);
        }

        glRotatef(-renderer->manager->player_rot, 0, 1, 0);
        tesselator_begin_quads();
        tesselator_normal(0, 1, 0);
        tesselator_vertex_uv(-0.5, -0.25, 0, u0, v1);
        tesselator_vertex_uv(0.5, -0.25, 0, u1, v1);
        tesselator_vertex_uv(0.5, 0.75, 0, u1, v0);
        tesselator_vertex_uv(-0.5, 0.75, 0, u0, v0);
        tesselator_end();
        glPopMatrix();
    }

    glDisable(GL_NORMALIZE);
    glPopMatrix();
}