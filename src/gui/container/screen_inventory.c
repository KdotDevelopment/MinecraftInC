#include <gui/container/screen_inventory.h>

#include <gui/container/inventory_crafting.h>
#include <gui/container/inventory_crafting_result.h>
#include <gui/container/screen_container.h>
#include <gui/container/slot_armor.h>
#include <gui/container/slot_crafting.h>
#include <minecraft.h>
#include <renderer/renderer_lighting.h>

void screen_inventory_create(screen_t *screen, inventory_t *inventory) {
    *screen = screen_container_create();

    screen->crafting_inv = inventory_crafting_create(screen, 2, 2);
    screen->crafting_result = inventory_crafting_result_create();

    slot_t crafting_slot = slot_crafting_create(screen, &screen->crafting_inv, &screen->crafting_result, 0, 144, 36);
    screen->inventory_slots = array_list_push(screen->inventory_slots, &crafting_slot);

    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {
            slot_t slot = slot_create(screen, &screen->crafting_inv, i + (j << 1), 88 + j * 18, 26 + i * 18);
            screen->inventory_slots = array_list_push(screen->inventory_slots, &slot);
        }
    }

    for(int i = 0; i < 4; i++) {
        slot_t slot = slot_armor_create(screen, inventory, inventory->size - 1 - i, 8, 8 + i * 18, i);
        screen->inventory_slots = array_list_push(screen->inventory_slots, &slot);
    }

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 9; j++) {
            slot_t slot = slot_create(screen, inventory, j + (i + 1) * 9, 8 + j * 18, 84 + i * 18);
            screen->inventory_slots = array_list_push(screen->inventory_slots, &slot);
        }
    }

    for(int i = 0; i < 9; i++) {
        slot_t slot = slot_create(screen, inventory, i, 8 + i * 18, 142);
        screen->inventory_slots = array_list_push(screen->inventory_slots, &slot);
    }

    screen->on_close = screen_inventory_on_close;
    screen->update_crafting = screen_inventory_update_crafting;
    screen->render = screen_inventory_render;
    screen->render_container_foreground = screen_inventory_render_container_foreground;
    screen->render_container_background = screen_inventory_render_container_background;
}

void screen_inventory_on_close(screen_t *screen) {
    screen_on_close(screen);

    for(int i = 0; i < screen->crafting_inv.size; i++) {
        item_stack_t item = screen->crafting_inv.get_slot(&screen->crafting_inv, i);
        if(item.item_id != 0) {
            // player drop item
        }
    }
}

void screen_inventory_update_crafting(screen_t *screen) {
    int16_t items[9];
    memset(items, 0, 18);
    for(int x = 0; x < 3; x++) {
        for(int y = 0; y < 3; y++) {
            int16_t result = -1;
            if(x < 2 && y < 2) {
                item_stack_t item = screen->crafting_inv.get_slot(&screen->crafting_inv, x + (y << 1));
                if(item.item_id != 0) {
                    result = item.item_id;
                }
            }

            items[x * 3 + y] = result;
        }
    }

    screen->crafting_result.set_slot(&screen->crafting_result, 0, crafting_manager_craft(&screen->minecraft->crafting_manager, items));
}

void screen_inventory_render(screen_t *screen, int mx, int my, float partial_tick) {
    screen_container_render(screen, mx, my, partial_tick);
    screen->mx = mx;
    screen->my = my;
}

void screen_inventory_render_container_foreground(screen_t *screen) {
    font_render_no_shadow(&screen->minecraft->font, "Crafting", 86, 16, 0x404040);
}

void screen_inventory_render_container_background(screen_t *screen) {
    glColor4f(1, 1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, textures_load(&screen->minecraft->textures, "gui/inventory.png"));
    int width = (screen->width - screen->container_x) / 2;
    int height = (screen->height - screen->container_y) / 2;
    gui_blit(width, height, 0, 0, screen->container_x, screen->container_y, 0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glPushMatrix();
    glTranslatef(width + 51, height + 75, 50);
    glScalef(-30, 30, 30);
    glRotatef(180, 0, 0, 1);
    float rot_body = screen->minecraft->player.mob->y_body_rot;
    float rot_x = screen->minecraft->player.x_rot;
    float rot_y = screen->minecraft->player.y_rot;
    float dmx = (width + 51) - screen->mx;
    float dmy = (height + 75 - 50) - screen->my;
    glRotatef(135, 0, 1, 0);
    renderer_lighting_enable();
    glRotatef(-135, 0, 1, 0);
    glRotatef(-atanf(dmy / 40.0) * 20.0, 1, 0, 0);
    screen->minecraft->player.mob->y_body_rot = atanf(dmx / 40.0) * 20.0;
    screen->minecraft->player.x_rot = atanf(dmx / 40.0) * 40.0;
    screen->minecraft->player.y_rot = -atanf(dmy / 40.0) * 20.0;
    glTranslatef(0, screen->minecraft->player.y_slide_offset, 0);
    renderer_entity_manager_render_pos(&screen->minecraft->entity_manager, &screen->minecraft->player, 0, 0, 0, 0, 1);
    screen->minecraft->player.mob->y_body_rot = rot_body;
    screen->minecraft->player.x_rot = rot_x;
    screen->minecraft->player.y_rot = rot_y;
    glPopMatrix();
    renderer_lighting_disable();
    glDisable(GL_NORMALIZE);
}