#pragma once

#include <gui/container/inventory.h>
#include <gui/screen.h>
#include <world/block/tile_entity/tile_entity.h>

void screen_chest_create(screen_t *screen, inventory_t *inventory, tile_entity_t *chest);
void screen_chest_render_container_foreground(screen_t *screen);
void screen_chest_render_container_background(screen_t *screen);