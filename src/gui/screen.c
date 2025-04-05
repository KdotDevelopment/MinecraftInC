#include <gui/screen.h>
#include <gui/gui.h>
#include <minecraft.h>

#include <util/array_list.h>

#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

screen_t screen_create() {
    screen_t screen = { 0 };
    screen.grabs_mouse = 0;
    screen.buttons = array_list_create(sizeof(button_t));

    screen.render = screen_render;
    screen.on_key_pressed = screen_on_key_pressed;
    screen.on_mouse_clicked = screen_on_mouse_clicked;
    screen.on_button_clicked = screen_on_button_clicked;
    screen.on_open = screen_on_open;
    screen.tick = screen_tick;
    screen.on_close = screen_on_close;
    screen.destroy = screen_destroy;

    return screen;
}

void screen_render(struct screen_s *proto_screen, int mouse_x, int mouse_y) {
    screen_t *screen = (screen_t *)proto_screen;

    for(int i = 0; i < array_list_length(screen->buttons); i++) {
        button_t *button = &screen->buttons[i];
        if(button->visible) {
            glBindTexture(GL_TEXTURE_2D, textures_load(&((minecraft_t *)screen->minecraft)->textures, "gui/gui.png"));
            glColor4f(1.0, 1.0, 1.0, 1.0);
            uint8_t hovered = mouse_x >= button->x && mouse_y >= button->y && mouse_x < button->x + button->width && mouse_y < button->y + button->height;
            int state = 1;

            if(!button->active) state = 0;
            else if(hovered) state = 2;

            gui_blit(button->x, button->y, 0, 46 + state * 20, button->width / 2, button->height, 0.0);
            gui_blit(button->x + button->width / 2, button->y, 200 - button->width / 2, 46 + state * 20, button->width / 2, button->height, 0.0);
            if(!button->active) {
                gui_draw_centered_string(screen->font, button->string, button->x + button->width / 2, button->y + (button->height - 8) / 2, 0xa0a0a0ff);
            }else if(hovered) {
                gui_draw_centered_string(screen->font, button->string, button->x + button->width / 2, button->y + (button->height - 8) / 2, 0xffffa0ff);
            }else {
                gui_draw_centered_string(screen->font, button->string, button->x + button->width / 2, button->y + (button->height - 8) / 2, 0xe0e0e0ff);
            }
        }
    }
}

void screen_on_key_pressed(struct screen_s *proto_screen, char event_char, int event_key) {
    screen_t *screen = (screen_t *)proto_screen;
    if(event_key == SDL_SCANCODE_ESCAPE) {
        minecraft_grab_mouse(screen->minecraft);
        minecraft_set_current_screen(screen->minecraft, NULL);
    }
}

void screen_on_mouse_clicked(struct screen_s *proto_screen, int x, int y, int button) {
    screen_t *screen = (screen_t *)proto_screen;
    if(button == SDL_BUTTON_LEFT) {
        for(int i = 0; i < array_list_length(screen->buttons); i++) {
            button_t *button = &screen->buttons[i];
            if(button->active && x >= button->x && y >= button->y && x < button->x + button->width && y < button->y + button->height) {
                screen->on_button_clicked(proto_screen, button);
                if(screen->removed) break;
            }
        }
    }
}

void screen_on_button_clicked(struct screen_s *proto_screen, button_t *button) {
    return;
}

void screen_open(screen_t *screen, struct minecraft_s *minecraft, int width, int height) {
    screen->minecraft = minecraft;
    screen->font = &minecraft->font;
    screen->width = width;
    screen->height = height;
    screen->on_open((struct screen_s *)screen);
}

void screen_on_open(struct screen_s *proto_screen) {
    return;
}

void screen_do_input(screen_t *screen, SDL_Event *events) {
    for(int i = 0; i < array_list_length(events); i++) {
        screen_mouse_event(screen, events[i]);
        screen_keyboard_event(screen, events[i]);
    }
}

void screen_mouse_event(screen_t *screen, SDL_Event event) {
    if(screen == NULL || screen->buttons == NULL) return;
    if(event.type == SDL_MOUSEBUTTONUP) {
        int x = event.button.x * screen->width / screen->minecraft->width;
		int y = event.button.y * screen->height / screen->minecraft->height - 1;
        screen->on_mouse_clicked((struct screen_s *)screen, x, y, event.button.button);
    }
}

void screen_keyboard_event(screen_t *screen, SDL_Event event) {
    if(event.type == SDL_KEYDOWN) {
        screen->on_key_pressed((struct screen_s *)screen, 0, event.key.keysym.scancode);
        return;
    }
    if(event.type == SDL_TEXTINPUT) {
        screen->on_key_pressed((struct screen_s *)screen, event.text.text[0], 0);
    }
}

void screen_tick(struct screen_s *proto_screen) {
    return;
}

void screen_on_close(struct screen_s *proto_screen) {
    screen_t *screen = (screen_t *)proto_screen;
    screen->removed = 1;
    return;
}

void screen_destroy(struct screen_s *proto_screen) {
    screen_t *screen = (screen_t *)proto_screen;
    for(int i = 0; i < array_list_length(screen->buttons); i++) {
        button_destroy(&screen->buttons[i]);
    }
    array_list_free(screen->buttons);
    screen->buttons = NULL;
}