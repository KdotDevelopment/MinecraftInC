#include <gui/container/screen_crafting.h>

#include <gui/container/inventory_crafting.h>
#include <gui/container/inventory_crafting_result.h>
#include <gui/container/screen_container.h>
#include <gui/container/slot_crafting.h>
#include <minecraft.h>

void screen_crafting_create(screen_t *screen, inventory_t *inventory) {
    *screen = screen_container_create();

    screen->crafting_inv = inventory_crafting_create(screen, 3, 3);
    screen->crafting_result = inventory_crafting_result_create();

    slot_t crafting_slot = slot_crafting_create(screen, &screen->crafting_inv, &screen->crafting_result, 0, 124, 35);
    screen->inventory_slots = array_list_push(screen->inventory_slots, &crafting_slot);

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            slot_t slot = slot_create(screen, &screen->crafting_inv, i * 3 + j, j * 18 + 30, i * 18 + 17);
            screen->inventory_slots = array_list_push(screen->inventory_slots, &slot);
        }
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

    screen->on_close = screen_crafting_on_close;
    screen->update_crafting = screen_crafting_update_crafting;
    screen->render_container_foreground = screen_crafting_render_container_foreground;
    screen->render_container_background = screen_crafting_render_container_background;
}

void screen_crafting_on_close(screen_t *screen) {
    screen_on_close(screen);

    for(int i = 0; i < screen->crafting_inv.size; i++) {
        item_stack_t item = screen->crafting_inv.get_slot(&screen->crafting_inv, i);
        if(item.item_id != 0) {
            // player drop item
        }
    }
}

void screen_crafting_update_crafting(screen_t *screen) {
    int16_t items[9];
    memset(items, 0, 18);
    for(int x = 0; x < 3; x++) {
        for(int y = 0; y < 3; y++) {
            int16_t result = -1;
            item_stack_t item = screen->crafting_inv.get_slot(&screen->crafting_inv, x * 3 + y);
            if(item.item_id != 0) {
                result = item.item_id;
            }

            items[x * 3 + y] = result;
        }
    }

    screen->crafting_result.set_slot(&screen->crafting_result, 0, crafting_manager_craft(&screen->minecraft->crafting_manager, items));
}

void screen_crafting_render_container_foreground(screen_t *screen) {
    font_render_no_shadow(&screen->minecraft->font, "Crafting", 28, 6, 0x404040);
    font_render_no_shadow(&screen->minecraft->font, "Inventory", 8, screen->container_y - 96 + 2, 0x404040);
}

void screen_crafting_render_container_background(screen_t *screen) {
    glColor4f(1, 1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, textures_load(&screen->minecraft->textures, "gui/crafting.png"));
    int width = (screen->width - screen->container_x) / 2;
    int height = (screen->height - screen->container_y) / 2;
    gui_blit(width, height, 0, 0, screen->container_x, screen->container_y, 0);
}