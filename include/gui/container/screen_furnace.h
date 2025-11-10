#pragma once

#include <gui/container/inventory.h>
#include <gui/screen.h>
#include <world/block/tile_entity/tile_entity.h>

void screen_furnace_create(screen_t *screen, inventory_t *inventory, tile_entity_t *furnace);
void screen_furnace_render_container_foreground(screen_t *screen);
void screen_furnace_render_container_background(screen_t *screen);