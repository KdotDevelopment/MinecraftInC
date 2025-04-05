#include <gui/screen_level_name.h>
#include <minecraft.h>

screen_t screen_level_name_create(screen_t *parent, char *name, int id) {
    screen_t screen = screen_create();
    screen.type = SCREEN_TYPE_LEVEL_NAME;
    screen.parent = (struct screen_s *)parent;
    screen.name = string_create(name);
    screen.id = id;
    screen.counter = 0;
    //screen.grabs_mouse = 1;
    screen.title = "Enter level name:";
    if(strcmp(name, "---") == 0) string_set(&screen.name, "");

    screen.on_open = screen_level_name_on_open;
    screen.on_close = screen_level_name_on_close;
    screen.tick = screen_level_name_tick;
    screen.render = screen_level_name_render;
    screen.on_key_pressed = screen_level_name_on_key_pressed;
    screen.on_button_clicked = screen_level_name_on_button_clicked;
    screen.destroy = screen_level_name_destroy;

    return screen;
}

void screen_level_name_on_open(struct screen_s *proto_screen) {
    screen_t *screen = (screen_t *)proto_screen;
    button_t button_save = button_create(0, screen->width / 2 - 100, screen->height / 4 + 120, "Save");
    button_t button_cancel = button_create(1, screen->width / 2 - 100, screen->height / 4 + 144, "Cancel");
    screen->buttons = array_list_push(screen->buttons, &button_save);
    screen->buttons = array_list_push(screen->buttons, &button_cancel);
    screen->buttons[0].active = string_length(screen->name) > 0;
}

void screen_level_name_on_close(struct screen_s *proto_screen) {
    return;
}

void screen_level_name_tick(struct screen_s *proto_screen) {
    screen_tick(proto_screen);
}

void screen_level_name_render(struct screen_s *proto_screen, int mouse_x, int mouse_y) {
    screen_t *screen = (screen_t *)proto_screen;
    gui_fill_gradient(0, 0, screen->width, screen->height, 0x05050060, 0x303060A0);
    gui_draw_centered_string(screen->font, screen->title, screen->width / 2, 40, 0xFFFFFFFF);
    int x = screen->width / 2 - 100;
	int y = screen->height / 2 - 10;
    gui_fill(x - 1, y - 1, x + 201, y + 21, 0xA0A0A0FF);
    gui_fill(x, y, x + 200, y + 20, 0x000000FF);
    char *string = string_create(screen->name);
    string_concat(&string, screen->counter / 6 % 2 == 0 ? "_" : "");
    gui_draw_string(screen->font, string, x + 4, y + 6, 0xE0E0E0FF);
    string_free(string);
}

void screen_level_name_on_key_pressed(struct screen_s *proto_screen, char event_char, int event_key) {
    screen_t *screen = (screen_t *)proto_screen;
    if(event_key == SDL_SCANCODE_BACKSPACE && string_length(screen->name) > 0) {
        screen->name = string_sub(screen->name, 0, string_length(screen->name) - 2);
    }
    char *allowed_chars = string_create("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ,.:-_\'*!\\\"#%%/()=+?[]{}<>@|$;");
    if(string_index_of(allowed_chars, event_char) >= 0 && string_length(screen->text) < 64 - 2) {
        string_concat(&screen->text, (char[]){ event_char, '\0' });
    }
    string_free(allowed_chars);
    screen->buttons[0].active = string_length(screen->name) > 0;
}

void screen_level_name_on_button_clicked(struct screen_s *proto_screen, button_t *button) {
    if(!button->active) return;
    screen_t *screen = (screen_t *)proto_screen;
    if(button->id == 0 && string_length(screen->name) > 0) {
        // file path stuff
        // not gonna worry about load and save now
        minecraft_set_current_screen(screen->minecraft, (screen_t *)screen->parent); // should be null when finished
    }

    // Cancel
    if(button->id == 1) {
        minecraft_set_current_screen(screen->minecraft, (screen_t *)screen->parent);
    }
}

void screen_level_name_destroy(struct screen_s *proto_screen) {
    screen_t *screen = (screen_t *)proto_screen;
    string_free(screen->name);

    screen_destroy(proto_screen);
}