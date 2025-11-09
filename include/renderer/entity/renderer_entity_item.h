#pragma once

#include <gui/font.h>
#include <minecraft.h>
#include <renderer/entity/renderer_entity.h>

renderer_entity_t renderer_entity_item_create();
void renderer_entity_item_render_gui(minecraft_t *minecraft, item_stack_t *item, int x, int y);
void renderer_entity_item_render_overlay_gui(font_t *font, item_stack_t *item, int x, int y);
void renderer_entity_item_render(renderer_entity_t *renderer, entity_t *entity, double x, double y, double z, float rot_x, float partial_tick);