#pragma once

#include <gui/container/inventory.h>
#include <gui/screen.h>

void screen_crafting_create(screen_t *screen, inventory_t *inventory);
void screen_crafting_on_close(screen_t *screen);
void screen_crafting_update_crafting(screen_t *screen);
void screen_crafting_render_container_foreground(screen_t *screen);
void screen_crafting_render_container_background(screen_t *screen);