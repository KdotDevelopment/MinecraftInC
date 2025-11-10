#include <gui/container/screen_furnace.h>

#include <gui/container/screen_container.h>
#include <gui/container/slot.h>
#include <minecraft.h>
#include <world/block/tile_entity/tile_entity_furnace.h>

void screen_furnace_create(screen_t *screen, inventory_t *inventory, tile_entity_t *furnace) {
    *screen = screen_container_create();

    screen->tile_entity = furnace;

    slot_t ingredient = slot_create(screen, &furnace->inventory, 0, 56, 17);
    screen->inventory_slots = array_list_push(screen->inventory_slots, &ingredient);

    slot_t fuel = slot_create(screen, &furnace->inventory, 1, 56, 53);
    screen->inventory_slots = array_list_push(screen->inventory_slots, &fuel);

    slot_t result = slot_create(screen, &furnace->inventory, 2, 116, 35);
    screen->inventory_slots = array_list_push(screen->inventory_slots, &result);

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

    screen->render_container_foreground = screen_furnace_render_container_foreground;
    screen->render_container_background = screen_furnace_render_container_background;
}

void screen_furnace_render_container_foreground(screen_t *screen) {
    font_render_no_shadow(&screen->minecraft->font, "Furnace", 60, 6, 0x404040);
    font_render_no_shadow(&screen->minecraft->font, "Inventory", 8, screen->container_y - 96 + 2, 0x404040);
}

void screen_furnace_render_container_background(screen_t *screen) {
    glColor4f(1, 1, 1, 1);
    glBindTexture(GL_TEXTURE_2D, textures_load(&screen->minecraft->textures, "gui/furnace.png"));
    int width = (screen->width - screen->container_x) / 2;
    int height = (screen->height - screen->container_y) / 2;
    gui_blit(width, height, 0, 0, screen->container_x, screen->container_y, 0);
    if(tile_entity_furnace_is_burning(screen->tile_entity) > 0) {
        int time = tile_entity_furnace_get_burn_time_remaining_scaled(screen->tile_entity, 12);
        gui_blit(width + 56, height + 36 + 12 - time, 176, 12 - time, 14, time + 2, 0);
    }
    int progress = tile_entity_furnace_get_cook_progress_scaled(screen->tile_entity, 24);
    gui_blit(width + 79, height + 34, 176, 14, progress + 1, 16, 0);
}