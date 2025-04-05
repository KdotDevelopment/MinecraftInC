#include <gui/screen_chat_input.h>
#include <gui/gui.h>
#include <minecraft.h>

#include <util/string.h>

screen_t screen_chat_input_create() {
    screen_t screen = screen_create();
    screen.type = SCREEN_TYPE_CHAT_INPUT;
    screen.text = string_create("");
    screen.counter = 0;

    screen.tick = screen_chat_input_tick;
    screen.render = screen_chat_input_render;
    screen.on_key_pressed = screen_chat_input_on_key_pressed;
    screen.on_mouse_clicked = screen_chat_input_on_mouse_clicked;
    screen.destroy = screen_chat_input_destroy;

    return screen;
}

void screen_chat_input_tick(struct screen_s *proto_screen) {
    screen_t *screen = (screen_t *)proto_screen;
    screen->counter++;
}

void screen_chat_input_render(struct screen_s *proto_screen, int mx, int my) {
    screen_t *screen = (screen_t *)proto_screen;
    gui_fill(2, screen->height - 14, screen->width - 2, screen->height - 2, 0x00000080);
    char *message = string_create(screen->text);
    string_concat_front("> ", &message);
    string_concat(&message, screen->counter / 6 % 2 == 0 ? "_" : " ");
    gui_draw_string(screen->font, message, 4, screen->height - 12, 0xE0E0E0FF);
    string_free(message);

    screen_render(proto_screen, mx, my);
}

void screen_chat_input_on_key_pressed(struct screen_s *proto_screen, char event_char, int event_key) {
    screen_t *screen = (screen_t *)proto_screen;
    if(event_key == SDL_SCANCODE_RETURN || event_key == SDL_SCANCODE_ESCAPE) {
        minecraft_set_current_screen(screen->minecraft, NULL);
        return;   
    }
    if(event_key == SDL_SCANCODE_BACKSPACE && string_length(screen->text) > 0) {
        screen->text = string_sub(screen->text, 0, string_length(screen->text) - 2);
    }
    char *allowed_chars = string_create("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ,.:-_\'*!\\\"#%%/()=+?[]{}<>@|$;");
    if(string_index_of(allowed_chars, event_char) >= 0 && string_length(screen->text) < 64 - 2) {
        string_concat(&screen->text, (char[]){ event_char, '\0' });
    }
    string_free(allowed_chars);
}

void screen_chat_input_on_mouse_clicked(struct screen_s *proto_screen, int x, int y, int button) {
    /*screen_t *screen = (screen_t *)proto_screen;
    if(button == SDL_BUTTON_LEFT && screen->minecraft->hud->hovered_player != NULL) {

    }*/
    // This is for clicking on a players name in the chat... not implemented
}

void screen_chat_input_destroy(struct screen_s *proto_screen) {
    screen_t *screen = (screen_t *)proto_screen;
    string_free(screen->text);
    screen_destroy(proto_screen);
}