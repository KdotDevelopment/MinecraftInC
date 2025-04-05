#include <gui/screen_save_level.h>

screen_t screen_save_level_create(screen_t *parent) {
    screen_t screen = screen_create();
    screen.type = SCREEN_TYPE_SAVE_LEVEL;
    //screen.grabs_mouse = 1;
    screen.parent = (struct screen_s *)parent;

    screen.on_open = screen_save_level_on_open;

    return screen;
}

void screen_save_level_on_open(struct screen_s *proto_screen) {
    screen_t *screen = (screen_t *)proto_screen;
    string_set(&screen->buttons[5].string, "Save file...");
}

void screen_save_level_open_level(struct screen_s *screen, int level) {
    return;
}

void screen_save_level_open_level_from_file(struct screen_s *screen, char *file) {
    return;
}