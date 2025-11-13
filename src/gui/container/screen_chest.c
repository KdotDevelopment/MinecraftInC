#include <gui/container/screen_chest.h>

#include <gui/container/screen_container.h>
#include <gui/container/slot.h>
#include <minecraft.h>
#include <world/block/tile_entity/tile_entity_chest.h>

void screen_chest_create(screen_t *screen, inventory_t *inventory, tile_entity_t *chest) {
    *screen = screen_container_create();

    screen->tile_entity = chest;
    screen->player_inv = inventory;
    screen->rows = screen->tile_entity->chest_inventory.size / 9;
    screen->container_y = 114 + screen->rows * 18;
    int offset = (screen->rows - 4) * 18;

    for(int i = 0; i < screen->rows; i++) {
        for(int j = 0; j < 9; j++) {
            slot_t slot = slot_create(screen, &chest->chest_inventory, i * 9 + j, j * 18 + 8, i * 18 + 18);
            screen->inventory_slots = array_list_push(screen->inventory_slots, &slot);
        }
    }

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 9; j++) {
            slot_t slot = slot_create(screen, inventory, j + (i + 1) * 9, 8 + j * 18, 103 + i * 18 + offset);
            screen->inventory_slots = array_list_push(screen->inventory_slots, &slot);
        }
    }

    for(int i = 0; i < 9; i++) {
        slot_t slot = slot_create(screen, inventory, i, 8 + i * 18, 161 + offset);
        screen->inventory_slots = array_list_push(screen->inventory_slots, &slot);
    }

    screen->render_container_foreground = screen_chest_render_container_foreground;
    screen->render_container_background = screen_chest_render_container_background;
}

void screen_chest_render_container_foreground(screen_t *screen) {
    font_render_no_shadow(&screen->minecraft->font, screen->tile_entity->chest_inventory.get_name(), 8, 6, 0x404040);
    font_render_no_shadow(&screen->minecraft->font, screen->player_inv->get_name(), 8, screen->container_y - 96 + 2, 0x404040);
}

void screen_chest_render_container_background(screen_t *screen) {
    glColor4f(1, 1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, textures_load(&screen->minecraft->textures, "gui/container.png"));
    int width = (screen->width - screen->container_x) / 2;
    int height = (screen->height - screen->container_y) / 2;
    gui_blit(width, height, 0, 0, screen->container_x, screen->rows * 18 + 17, 0);
    gui_blit(width, height + screen->rows * 18 + 17, 0, 126, screen->container_x, 96, 0);
}