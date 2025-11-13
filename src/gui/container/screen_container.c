#include <gui/container/screen_container.h>

#include <item/item.h>
#include <minecraft.h>
#include <renderer/entity/renderer_entity_item.h>
#include <renderer/renderer_lighting.h>

#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

screen_t screen_container_create() {
    screen_t screen = screen_create();
    screen.type = SCREEN_TYPE_CONTAINER;
    screen.pauses_game = 0;
    screen.inventory_slots = array_list_create(sizeof(slot_t));
    screen.container_x = 176;
    screen.container_y = 166;

    screen.render = screen_container_render;
    screen.on_mouse_clicked = screen_container_on_mouse_clicked;
    screen.on_key_pressed = screen_container_on_key_pressed;
    screen.destroy = screen_container_destroy;

    return screen;
}

void screen_container_render(screen_t *screen, int mx, int my, float partial_tick) {
    screen_render_background(screen);
    int width = (screen->width - screen->container_x) / 2;
    int height = (screen->height - screen->container_y) / 2;
    screen->render_container_background(screen);
    glPushMatrix();
    glRotatef(180, 1, 0, 0);
    renderer_lighting_enable();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(width, height, 0);
    glColor4f(1, 1, 1, 1);
    glEnable(GL_NORMALIZE);

    for(int i = 0; i < array_list_length(screen->inventory_slots); i++) {
        slot_t *slot = (slot_t *)array_list_get(screen->inventory_slots, i);
        inventory_t *inv = slot->inventory;
        int index = slot->index;
        int slot_x = slot->x;
        int slot_y = slot->y;
        item_stack_t item = inv->get_slot(inv, index);

        if(item.item_id == 0) {
            int texture = slot->get_texture(slot);
            if(texture >= 0) {
                glDisable(GL_LIGHTING);
                glBindTexture(GL_TEXTURE_2D, textures_load(&screen->minecraft->textures, "gui/items.png"));
                gui_blit(slot_x, slot_y, texture % 16 << 4, texture / 16 << 4, 16, 16, 0);
                glEnable(GL_LIGHTING);
            }
        }else {
            renderer_entity_item_render_gui(screen->minecraft, &item, slot_x, slot_y);
            renderer_entity_item_render_overlay_gui(&screen->minecraft->font, &item, slot_x, slot_y);
        }

        if(slot_is_under_cursor(slot, mx, my)) {
            glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            gui_fill_gradient(slot->x, slot->y, slot->x + 16, slot->y + 16, 0xFFFFFF80, 0xFFFFFF80);
            glDisable(GL_BLEND);
            glEnable(GL_LIGHTING);
            glEnable(GL_DEPTH_TEST);
        }
    }

    if(screen->held_item.item_id != 0) {
        glTranslatef(0, 0, 32);
        renderer_entity_item_render_gui(screen->minecraft, &screen->held_item, mx - width - 8, my - height - 8);
        renderer_entity_item_render_overlay_gui(&screen->minecraft->font, &screen->held_item, mx - width - 8, my - height - 8);
    }

    glDisable(GL_NORMALIZE);
    renderer_lighting_disable();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    screen->render_container_foreground(screen);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
}

void screen_container_on_mouse_clicked(screen_t *screen, int mx, int my, int button) {
    if(button == SDL_BUTTON_LEFT || button == SDL_BUTTON_RIGHT) {
        slot_t *slot = NULL;
        for(int i = 0; i < array_list_length(screen->inventory_slots); i++) {
            slot_t *index = (slot_t *)array_list_get(screen->inventory_slots, i);
            if(slot_is_under_cursor(index, mx, my)) {
                slot = index;
                break;
            }
        }

        if(slot != NULL) {
            slot->on_changed(slot);
            item_stack_t item = slot->inventory->get_slot(slot->inventory, slot->index);
            if(item.item_id == 0 && screen->held_item.item_id == 0) {
                return;
            }

            // picking up
            if(item.item_id != 0 && screen->held_item.item_id == 0) {
                int pickup_count = 0;
                if(button == SDL_BUTTON_LEFT) {
                    pickup_count = item.stack_size;
                }else {
                    pickup_count = (item.stack_size + 1) / 2;
                }

                screen->held_item = slot->inventory->remove_item(slot->inventory, slot->index, pickup_count);
                item = slot->inventory->get_slot(slot->inventory, slot->index);
                if(item.stack_size == 0) {
                    slot->inventory->set_slot(slot->inventory, slot->index, item_stack_create(0, 0, 0));
                }

                slot->on_pickup(slot);
            }else if(item.item_id == 0 && screen->held_item.item_id != 0 && slot->can_put_item(slot, screen->held_item)) {
                // placing
                int place_count = 0;
                if(button == SDL_BUTTON_LEFT) {
                    place_count = screen->held_item.stack_size;
                }else {
                    place_count = 1;
                }

                if(place_count > slot->inventory->stack_limit) {
                    place_count = slot->inventory->stack_limit;
                }

                slot_put_stack(slot, item_stack_split(&screen->held_item, place_count));
                if(screen->held_item.stack_size == 0) {
                    screen->held_item = item_stack_create(0, 0, 0);
                }
            }else {
                // cursor and slot both have items
                if(item.item_id == 0 || screen->held_item.item_id == 0) return;

                if(!slot->can_put_item(slot, screen->held_item)) {
                    if(item.item_id == screen->held_item.item_id) {
                        if(item_list[screen->held_item.item_id].max_stack_size > 1) {
                            int count = item.stack_size;
                            if(count > 0) {
                                int combined = count + screen->held_item.stack_size;
                                if(combined <= item_list[screen->held_item.item_id].max_stack_size) {
                                    screen->held_item.stack_size += count;
                                    slot->inventory->set_slot(slot->inventory, slot->index, item_stack_split(&item, count));
                                    item = slot->inventory->get_slot(slot->inventory, slot->index);
                                    if(item.stack_size == 0) {
                                        slot->inventory->set_slot(slot->inventory, slot->index, item_stack_create(0, 0, 0));
                                    }

                                    slot->on_pickup(slot);
                                    return;
                                }
                            }
                        }
                    }
                    return;
                }
                // swap
                if(item.item_id != screen->held_item.item_id) {
                    if(screen->held_item.stack_size > slot->inventory->stack_limit) {
                        return;
                    }
                    slot_put_stack(slot, screen->held_item);
                    screen->held_item = item;
                }else {
                    // combine two stacks
                    if(button == SDL_BUTTON_LEFT) {
                        int count = screen->held_item.stack_size;
                        if(count > slot->inventory->stack_limit - item.stack_size) {
                            count = slot->inventory->stack_limit - item.stack_size;
                        }

                        if(count > item_list[screen->held_item.item_id].max_stack_size - item.stack_size) {
                            count = item_list[screen->held_item.item_id].max_stack_size - item.stack_size;
                        }

                        item_stack_split(&screen->held_item, count);
                        if(screen->held_item.stack_size == 0) {
                            screen->held_item = item_stack_create(0, 0, 0);
                        }
                        item.stack_size += count;
                        slot->inventory->set_slot(slot->inventory, slot->index, item);
                    }else if(button == SDL_BUTTON_RIGHT) {
                        int count = 1;
                        if(count > slot->inventory->stack_limit - item.stack_size) {
                            count = slot->inventory->stack_limit - item.stack_size;
                        }

                        if(count > item_list[screen->held_item.item_id].max_stack_size - item.stack_size) {
                            count = item_list[screen->held_item.item_id].max_stack_size - item.stack_size;
                        }

                        item_stack_split(&screen->held_item, count);
                        if(screen->held_item.stack_size == 0) {
                            screen->held_item = item_stack_create(0, 0, 0);
                        }
                        item.stack_size += count;
                        slot->inventory->set_slot(slot->inventory, slot->index, item);
                    }
                }
            }
        }else if(screen->held_item.item_id != 0 && screen->held_item.stack_size != 0) {
            // cursor outside of inventory
            int width = (screen->width - screen->container_x) / 2;
            int height = (screen->height - screen->container_y) / 2;

            if(mx < width || my < height || mx >= width + screen->container_x || my >= height + screen->container_y) {
                //entity_t *player = &screen->minecraft->player;
                if(button == SDL_BUTTON_LEFT) {
                    // TODO player drop item
                    screen->held_item = item_stack_create(0, 0, 0);
                }

                if(button == SDL_BUTTON_RIGHT) {
                    // TODO player drop item
                    item_stack_split(&screen->held_item, 1);
                    if(screen->held_item.stack_size == 0) {
                        screen->held_item = item_stack_create(0, 0, 0);
                    }
                }
            }
        }
    }
}

void screen_container_on_key_pressed(screen_t *screen, char event_char, int event_key) {
    if(event_key == screen->minecraft->settings.inventory_key.key || event_key == SDL_SCANCODE_ESCAPE) {
        minecraft_set_current_screen(screen->minecraft, NULL);
    }
}

void screen_container_destroy(screen_t *screen) {
    array_list_free(screen->inventory_slots);
    screen_destroy(screen);
}