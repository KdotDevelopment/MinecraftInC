#pragma once

#include <gui/container/inventory.h>
#include <gui/screen.h>

void screen_inventory_create(screen_t *screen, inventory_t *inventory);
void screen_inventory_on_close(screen_t *screen);
void screen_inventory_update_crafting(screen_t *screen);
void screen_inventory_render(screen_t *screen, int mx, int my, float partial_tick);
void screen_inventory_render_container_foreground(screen_t *screen);
void screen_inventory_render_container_background(screen_t *screen);